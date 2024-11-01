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
  GPIOPin *flow_control_pin_{nullptr};

  public:
    void setup() {
      if (flow_control_pin_ != nullptr) {
        flow_control_pin_->setup();
        flow_control_pin_->digital_write(false);
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

    void set_flow_control_pin(GPIOPin *flow_control_pin) {
      flow_control_pin_ = flow_control_pin;
    }

    // buffer to store serial data
    std::vector<uint8_t> bytes;

    void update() {

      // check if we should request an update for any inverters
      check_next_update_request();

      // read data off UART
      while (available() > 0) {
        // add new bytes to buffer
        bytes.push_back(read());

        // the minimum amount of data for a valid packet is 9 bytes (Packet 3,
        // page 8)
        if (bytes.size() < 9) {
          continue;
        }

        // perform a quick check to see if this is a protocol header
        if (bytes[0] != STX || bytes[1] != ACK || bytes[2] == 0 || bytes[4] != 0x60 || bytes[5] != 0x01) {
          bytes.erase(bytes.begin());
          continue;
        }

        // read full packet
        unsigned int required_size = 6 + bytes[3] + 3;
        if (bytes.size() != required_size) {
          continue;
        }

        // should have a valid header here
        ESP_LOGD(LOG_TAG, "PACKET - header = %02x%02x%02x%02x%02x%02x", bytes[0], bytes[1], bytes[2], bytes[3], bytes[4], bytes[5]);
        ESP_LOGD(LOG_TAG, "       - SOP = 0x%02x, PC = 0x%02x, address = %u, data size = %u, cmd = 0x%02x, sub cmd = 0x%02x",
          bytes[0],
          bytes[1],
          bytes[2],
          bytes[3],
          bytes[4],
          bytes[5]
        );

        // validate End-of-Protocol byte
        const uint16_t end_of_data     = 5 + bytes[3] - 1;
        const uint8_t  end_of_protocol = bytes[end_of_data + 2];
        if (end_of_protocol != ETX) {
          ESP_LOGE(LOG_TAG, "PACKET - invalid end-of-protocol byte (was 0x%02x, should be 0x%02x)", end_of_protocol, ETX);
          bytes.clear();
          continue;
        }

        // validate CRC (page 8/9)
        const uint16_t packet_crc     = *reinterpret_cast<const uint16_t*>(&bytes[end_of_data]);
        uint16_t       calculated_crc = delta_solivia_crc(&bytes[1], &bytes[end_of_data - 1]);
        if (packet_crc != calculated_crc) {
          ESP_LOGE(LOG_TAG, "PACKET - CRC mismatch (was 0x%04X, should be 0x%04X)", calculated_crc, packet_crc);
          bytes.clear();
          continue;
        }

        // update inverter (if it's known and should be updated)
        auto inverter = get_inverter(bytes[2]);
        if (inverter->should_update_sensors()) {
          inverter->update_sensors(bytes);
        }

        // done
        bytes.clear();
      }

    }

    void check_next_update_request() {
      static InverterMap::const_iterator it = inverters.begin();
      auto inverter = it->second;

      // request update for the next inverter
      if (inverter->should_request_update()) {
        inverter->request_update(
          [this](const uint8_t* bytes, unsigned len) -> void {
            if (this->flow_control_pin_ != nullptr) {
              this->flow_control_pin_->digital_write(true);
            }
            this->write_array(bytes, len);
            this->flush();
            if (this->flow_control_pin_ != nullptr) {
              this->flow_control_pin_->digital_write(false);
            }
          }
        );
      }
      // wrap around when we reached the end
      if (++it == inverters.end()) {
        it = inverters.begin();
      }
    }
};

}
}
