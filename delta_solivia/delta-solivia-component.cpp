#include "delta-solivia-component.h"
#include "delta-solivia-crc.h"

namespace esphome {
namespace delta_solivia {

void DeltaSoliviaComponent::setup() {
  if (flow_control_pin != nullptr) {
    flow_control_pin->setup();
    flow_control_pin->digital_write(false);
  }
}

// add an inverter
void DeltaSoliviaComponent::add_inverter(DeltaSoliviaInverter* inverter) {
  ESP_LOGD(LOG_TAG, "CONFIG - added inverter with address %u", inverter->get_address());
  inverters[inverter->get_address()] = inverter;
}

// get inverter
DeltaSoliviaInverter* DeltaSoliviaComponent::get_inverter(uint8_t address) {
  return inverters.count(address) == 1 ? inverters[address] : nullptr;
}

// process an incoming packet
bool DeltaSoliviaComponent::process_frame(const Frame& frame) {
  if (! validate_header(frame)) {
    return false;
  }

  if (! validate_size(frame)) {
    return false;
  }

  if (! validate_trailer(frame)) {
    return false;
  }

  // update inverter
  auto inverter = get_inverter(frame[2]);
  if (inverter->should_update_sensors()) {
    // copy vector to plain byte buffer
    uint8_t buffer[frame.size()];
    std::copy(frame.begin(), frame.end(), buffer);

    inverter->update_sensors(buffer);
  }

  return true;
}

// validate packet header
bool DeltaSoliviaComponent::validate_header(const Frame& frame) {
  if (frame.size() < 6 || frame[0] != STX || frame[1] != ACK || frame[2] == 0 || frame[4] != 0x60 || frame[5] != 0x01) {
    ESP_LOGD(LOG_TAG, "FRAME - incorrect header");
    return false;
  }

  // validate address
  if (! validate_address(frame)) {
    return false;
  }

  ESP_LOGD(LOG_TAG, "FRAME - SOP = 0x%02x, PC = 0x%02x, address = %u, data size = %u, cmd = 0x%02x, sub cmd = 0x%02x",
    frame[0],
    frame[1],
    frame[2],
    frame[3],
    frame[4],
    frame[5]
  );
  return true;
}

bool DeltaSoliviaComponent::validate_size(const Frame& frame) {
  unsigned int expected = 4 + frame[3] + 3;

  if (frame.size() != expected) {
    ESP_LOGD(LOG_TAG, "FRAME - incorrect frame size (was %u, expected %u)", frame.size(), expected);
    return false;
  }
  return true;
}

bool DeltaSoliviaComponent::validate_address(const Frame& frame) {
  unsigned int address = frame[2];
  auto inverter        = get_inverter(address);

  if (inverter == nullptr) {
    ESP_LOGD(LOG_TAG, "FRAME - unknown address %u", address);
    return false;
  }
  return true;
}

bool DeltaSoliviaComponent::validate_trailer(const Frame& frame) {
  const uint16_t end_of_data     = frame[3] + 4;
  const uint8_t  end_of_protocol = frame[end_of_data + 2];

  if (end_of_protocol != ETX) {
    ESP_LOGE(LOG_TAG, "FRAME - invalid end-of-protocol byte (was 0x%02x, should be 0x%02x)", end_of_protocol, ETX);
    return false;
  }

  const uint16_t packet_crc     = *reinterpret_cast<const uint16_t*>(&frame[end_of_data]);
  uint16_t       calculated_crc = delta_solivia_crc(&frame[1], &frame[end_of_data - 1]);
  if (packet_crc != calculated_crc) {
    ESP_LOGE(LOG_TAG, "FRAME - CRC mismatch (was 0x%04X, should be 0x%04X)", calculated_crc, packet_crc);
    return false;
  }

  return true;
}

void DeltaSoliviaComponent::update() {
  if (has_gateway) {
    update_with_gateway();
  } else {
    update_without_gateway();
  }
}

void DeltaSoliviaComponent::update_without_gateway() {
  // toggle between inverters to query
  static InverterMap::const_iterator it = inverters.begin();
  auto inverter = it->second;

  // request an update from the inverter
  inverter->request_update(
    [this](const uint8_t* bytes, unsigned len) -> void {
      if (this->flow_control_pin != nullptr) {
        this->flow_control_pin->digital_write(true);
      }
      this->write_array(bytes, len);
      this->flush();
      if (this->flow_control_pin != nullptr) {
        this->flow_control_pin->digital_write(false);
      }
    }
  );

  // pick inverter for the next update, with wrap around
  if (++it == inverters.end()) {
    it = inverters.begin();
  }

  // protocol timing chart (page 9) says response should arrive within 6ms.
  uint32_t start = millis();
  while (millis() - start < 7 && ! available())
      ;

  if (! available()) {
    ESP_LOGD(LOG_TAG, "RESPONSE - timeout");
    return;
  }

  // absolute maximum packet size
  uint8_t buffer[261];

  // read packet (XXX: this assumes that the full packet is available in the receive buffer)
  unsigned int bytes_read = available();
  bool read_success       = read_array(buffer, bytes_read);

  if (! read_success) {
    ESP_LOGD(LOG_TAG, "RESPONSE - unable to read packet");
    return;
  }

  // process frame
  Frame frame(buffer, buffer + bytes_read);
  process_frame(frame);
}

void DeltaSoliviaComponent::update_with_gateway() {
  // buffer to store serial data
  static Frame frame;

  // read data off UART
  while (available() > 0) {
    // add new bytes to buffer
    frame.push_back(read());

    // wait until we've read enough bytes for a header
    if (frame.size() < 6) {
      continue;
    }

    // validate header
    if (! validate_header(frame)) {
      frame.erase(frame.begin());
      continue;
    }

    // read full packet
    unsigned int required_size = 4 + frame[3] + 3;
    if (frame.size() != required_size) {
      continue;
    }

    // process frame
    process_frame(frame);

    // clear vector for next round
    frame.clear();
  }
}

}
}
