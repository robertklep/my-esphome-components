// Based on Public Solar Inverter Communication Protocol (Version 1.2)
#pragma once

#include "esphome.h"
#include "esphome/components/uart/uart.h"
#include "delta-solivia-inverter.h"

namespace esphome {
namespace delta_solivia {

// logging tag
#define LOG_TAG "DeltaSolivia"

// protocol characters (page 7)
#define ENQ 0x05
#define ACK 0x06
#define NAK 0x15
#define STX 0x02
#define ETX 0x03

using uart::UARTDevice;
using uart::UARTComponent;

class DeltaSoliviaComponent: public PollingComponent, public UARTDevice {
  uint32_t throttle_ms = 10000;
  std::map<uint8_t, DeltaSoliviaInverter*> inverters;

  // Packet CRC calculation
  uint16_t calc_crc(uint8_t *sop, uint8_t *eop) {
    uint8_t *char_ptr = sop;
    uint16_t crc = 0x0000;

    do {
      uint8_t bit_count = 0;
      crc ^= ((*char_ptr) & 0x00ff);
      do {
        if (crc & 0x0001) {
          crc >>= 1;
          crc ^= 0xA001;
        } else {
          crc >>= 1;
        }
      } while ( bit_count++ < 7 );
    } while ( char_ptr++ < eop );
    return crc;
  }

  public:

    // set throttle interval
    void set_throttle(uint32_t ms) {
      ESP_LOGD(LOG_TAG, "CONFIG - setting throttle interval to %u ms", ms);
      throttle_ms = ms;
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

    // buffer to store serial data
    std::vector<uint8_t> bytes;

    void update() {
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
          //ESP_LOGI(LOG_TAG, "BYTES READ = %u NEED %u", bytes.size(), required_size);
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
        uint16_t       calculated_crc = calc_crc(&bytes[1], &bytes[end_of_data - 1]);
        if (packet_crc != calculated_crc) {
          ESP_LOGE(LOG_TAG, "PACKET - CRC mismatch (was 0x%04X, should be 0x%04X)", calculated_crc, packet_crc);
          bytes.clear();
          continue;
        }

        // update inverter (if it's known and should be updated)
        auto inverter = get_inverter(bytes[2]);

        if (inverter != nullptr && inverter->last_update_older_than(throttle_ms)) {
          inverter->update(bytes);
        }

        // done
        bytes.clear();
      }
    }
};

}
}
