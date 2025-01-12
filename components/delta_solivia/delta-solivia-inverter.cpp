#include "delta-solivia-inverter.h"
#include "parser-variant-15.h"

namespace esphome {
namespace delta_solivia {

DeltaSoliviaInverter::DeltaSoliviaInverter(uint8_t address, uint8_t variant) {
  address_ = address;
  variant_ = variant;
  if (variant == 15 || variant == 18 || variant == 19 || variant == 20 || variant == 31 || variant == 34 || variant == 35 || variant == 36 || variant == 38 || variant == 39 ||
      variant == 55 || variant == 58 || variant == 59 || variant == 60) {

    // instantiate parser
    parser = new Variant15Parser();

    // set handlers
    parser->set_publish_sensor([this](const std::string& name, float value) {
      this->publish_sensor(name, value, false);
    });
    parser->set_publish_text_sensor([this](const std::string& name, const std::string& value)  {
      this->publish_text_sensor(name, value, false);
    });
  } else {
    ESP_LOGE(LOG_TAG, "INVERTER%u - invalid variant %u", address_, variant_);
  }
}

void DeltaSoliviaInverter::update_sensors(const uint8_t* buffer) {
  ESP_LOGD(LOG_TAG, "INVERTER#%u - updating sensors", address_);
  parser->parse_frame(buffer, true);
}

void DeltaSoliviaInverter::publish_sensor(const std::string& name, float value, bool once) {
  ESP_LOGD(LOG_TAG, "INVERTER#%u - sensor '%s', value = %f", address_, name.c_str(), value);
  auto entry= sensors_.find(name);

  if (entry != sensors_.end()) {
    if (! once || ! entry->second->has_state()) {
      entry->second->publish_state(value);
    }
  }
}

void DeltaSoliviaInverter::publish_text_sensor(const std::string& name, const std::string& value, bool once) {
  ESP_LOGD(LOG_TAG, "INVERTER#%u - sensor '%s', value = %s", address_, name.c_str(), value.c_str());
  auto entry= text_sensors_.find(name);

  if (entry != text_sensors_.end()) {
    if (! once || ! entry->second->has_state()) {
      entry->second->publish_state(value);
    }
  }
}

}
}
