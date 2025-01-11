#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <functional>

namespace esphome {
namespace delta_solivia {

using PublishSensorHandler     = std::function<void(const std::string&, float)>;
using PublishTextSensorHandler = std::function<void(const std::string&, const std::string&)>;

class BaseParser {
  virtual void parseFrame_(const uint8_t* frame, std::size_t pos) = 0;

protected:
  int16_t extractInt16(const uint8_t *data) {
    return (data[0] << 8) | data[1];
  }

  uint32_t extractInt32(const uint8_t *data) {
    return (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
  }

  float applyScaling(int16_t value, float scale) {
    return value * scale;
  }

  float applyScaling(uint32_t value, float scale) {
    return value * scale;
  }

  std::string parseString(const uint8_t* frame, std::size_t pos, int length) {
    std::string result(frame + pos, frame + pos + length);
    return result;
  }

  PublishSensorHandler publishSensor_;
  PublishTextSensorHandler publishTextSensor_;
public:
  void parseFrame(const uint8_t* frame, bool skipHeader = false) {
    std::size_t pos = skipHeader ? 6 : 0;
    parseFrame_(frame, pos);
  }

  void setPublishSensor(PublishSensorHandler publishSensor) {
    publishSensor_ = publishSensor;
  }

  void setPublishTextSensor(PublishTextSensorHandler publishTextSensor) {
    publishTextSensor_ = publishTextSensor;
  }
};

}
}
