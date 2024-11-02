// Based on Public Solar Inverter Communication Protocol (Version 1.2)
#pragma once

#include <map>
#include "esphome.h"
#include "esphome/components/uart/uart.h"
#include "delta-solivia-inverter.h"
#include "delta-solivia-crc.h"

namespace esphome {
namespace delta_solivia {

using uart::UARTDevice;
using uart::UARTComponent;

typedef std::map<uint8_t, DeltaSoliviaInverter*> InverterMap;

class DeltaSoliviaComponent: public PollingComponent, public UARTDevice {
  InverterMap inverters;
  GPIOPin *flow_control_pin{nullptr};
  bool has_gateway;

  public:
    DeltaSoliviaComponent() : has_gateway(false) {}

    void setup() {
      if (flow_control_pin != nullptr) {
        flow_control_pin->setup();
        flow_control_pin->digital_write(false);
      }
    }

    // add an inverter
    void add_inverter(DeltaSoliviaInverter* inverter) {
      ESP_LOGD(LOG_TAG, "CONFIG - added inverter with address %u", inverter->get_address());
      inverters[inverter->get_address()] = inverter;
    }

    // get inverter
    DeltaSoliviaInverter* get_inverter(uint8_t address) {
      return inverters.count(address) == 1 ? inverters[address] : nullptr;
    }

    // set flow control pin (optional)
    void set_flow_control_pin(GPIOPin *flow_control_pin_) {
      flow_control_pin = flow_control_pin_;
    }

    // set has-gateway
    void set_has_gateway(bool has_gateway_) {
      has_gateway = has_gateway_;
    }

    bool process_packet(const uint8_t* buffer, unsigned int size) {
      if (! validate_header(buffer, size)) {
        return false;
      }

      if (! validate_size(buffer, size)) {
        return false;
      }

      if (! validate_trailer(buffer, size)) {
        return false;
      }

      // update inverter
      auto inverter = get_inverter(buffer[2]);
      if (inverter->should_update_sensors()) {
        inverter->update_sensors(buffer);
      }

      return true;
    }

    bool validate_header(const uint8_t* buffer, unsigned int size) {
      // validate header
      if (size < 6 || buffer[0] != STX || buffer[1] != ACK || buffer[2] == 0 || buffer[4] != 0x60 || buffer[5] != 0x01) {
        ESP_LOGD(LOG_TAG, "PACKET - incorrect header");
        return false;
      }

      // validate address
      if (! validate_address(buffer, size)) {
        return false;
      }

      ESP_LOGD(LOG_TAG, "PACKET - SOP = 0x%02x, PC = 0x%02x, address = %u, data size = %u, cmd = 0x%02x, sub cmd = 0x%02x",
        buffer[0],
        buffer[1],
        buffer[2],
        buffer[3],
        buffer[4],
        buffer[5]
      );
      return true;
    }

#define min(a, b) ((a < b) ? a : b)

    bool validate_header(const std::vector<uint8_t>& vec, unsigned int size) {
      uint8_t buffer[size];

      for (int i = 0; i < min(size, sizeof(buffer)); i++) {
        buffer[i] = vec[i];
      }
      return validate_header(buffer, size);
    }

    bool validate_size(const uint8_t* buffer, unsigned int size) {
      unsigned int expected = 4 + buffer[3] + 3;

      if (size != expected) {
        ESP_LOGD(LOG_TAG, "PACKET - incorrect packet size (was %u, expected %u)", size, expected);
        return false;
      }
      return true;
    }

    bool validate_address(const uint8_t* buffer, unsigned int size) {
      auto inverter = get_inverter(buffer[2]);

      if (inverter == nullptr) {
        ESP_LOGD(LOG_TAG, "PACKET - unknown address %u", buffer[2]);
        return false;
      }
      return true;
    }

    bool validate_trailer(const uint8_t* buffer, unsigned int size) {
      const uint16_t end_of_data     = buffer[3] + 4;
      const uint8_t  end_of_protocol = buffer[end_of_data + 2];

      if (end_of_protocol != ETX) {
        ESP_LOGE(LOG_TAG, "PACKET - invalid end-of-protocol byte (was 0x%02x, should be 0x%02x)", end_of_protocol, ETX);
        return false;
      }

      const uint16_t packet_crc     = *reinterpret_cast<const uint16_t*>(&buffer[end_of_data]);
      uint16_t       calculated_crc = delta_solivia_crc(&buffer[1], &buffer[end_of_data - 1]);
      if (packet_crc != calculated_crc) {
        ESP_LOGE(LOG_TAG, "PACKET - CRC mismatch (was 0x%04X, should be 0x%04X)", calculated_crc, packet_crc);
        return false;
      }

      return true;
    }

    void update() {
      if (has_gateway) {
        update_with_gateway();
      } else {
        update_without_gateway();
      }
    }

    void update_without_gateway() {
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

      // process packet
      process_packet(buffer, bytes_read);
    }

    void update_with_gateway() {
      // buffer to store serial data
      static std::vector<uint8_t> bytes;

      // read data off UART
      while (available() > 0) {
        // add new bytes to buffer
        bytes.push_back(read());

        // wait until we've read enough bytes for a header
        if (bytes.size() < 6) {
          continue;
        }

        // validate header
        if (! validate_header(bytes, bytes.size())) {
          bytes.erase(bytes.begin());
          continue;
        }

        // read full packet
        unsigned int required_size = 4 + bytes[3] + 3;
        if (bytes.size() != required_size) {
          continue;
        }

        // should have the full packet here, copy into array
        uint8_t buffer[bytes.size()];
        std::copy(bytes.begin(), bytes.end(), buffer);

        // clear vector for next round
        bytes.clear();

        // process packet
        process_packet(buffer, sizeof(buffer));
      }
    }

};

}
}
