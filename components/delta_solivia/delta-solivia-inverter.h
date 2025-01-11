#ifndef DELTA_SOLIVIA_INVERTER_H
#define DELTA_SOLIVIA_INVERTER_H

#include <map>
#include <string>
#include <cstdint>
#include "esphome.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "constants.h"
#include "delta-solivia-crc.h"
#include "parser-base.h"

namespace esphome {
namespace delta_solivia {

using sensor::Sensor;
using text_sensor::TextSensor;

using SensorMap     = std::map<std::string, Sensor*>;
using TextSensorMap = std::map<std::string, TextSensor*>;

class DeltaSoliviaInverter {
  protected:
    uint8_t address_;
    uint8_t variant_;
    SensorMap sensors_;
    TextSensorMap textSensors_;
    BaseParser* parser = nullptr;

  public:
    DeltaSoliviaInverter(uint8_t address, uint8_t variant);

    uint8_t get_address() {
      return address_;
    }

    void addSensor(const std::string& name, Sensor* sensor) {
      sensors_[name] = sensor;
    }

    void addTextSensor(const std::string& name, TextSensor* sensor) {
      textSensors_[name] = sensor;
    }

    void publishSensor(const std::string& name, float value, bool once = false);
    void publishTextSensor(const std::string& name, const std::string& value, bool once = false);
    void update_sensors(const uint8_t*);

    template <typename F>
    void request_update(const F& callback) {
      ESP_LOGD(LOG_TAG, "INVERTER%u - requesting update", address_);

      // Enquire packet (page 7/8)
      const uint8_t bytes[] = {
        STX,      // start of protocol
        ENQ,      // enquire
        address_, // for inverter with address
        0x02,     // number of data bytes, including commands
        0x60,     // command
        0x01,     // subcommand
        0x00,     // CRC low
        0x00,     // CRC high
        ETX       // end of protocol
      };

      // calculate CRC
      *((uint16_t*) &bytes[6]) = delta_solivia_crc((uint8_t *) bytes + 1, (uint8_t *) bytes + 5);

      // call callback with data, caller will handle writing to UART
      callback(&bytes[0], sizeof(bytes));
    }
};

}
}

#endif // DELTA_SOLIVIA_INVERTER_H
