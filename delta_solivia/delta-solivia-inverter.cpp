#include "delta-solivia-inverter.h"

namespace esphome {
namespace delta_solivia {

bool DeltaSoliviaInverter::should_update_sensors() {
  uint32_t now = millis();

  if (now - last_update < update_interval) {
    return false;
  }
  return true;
}

void DeltaSoliviaInverter::update_sensors(const uint8_t* buffer) {
  ESP_LOGD(LOG_TAG, "INVERTER#%u - updating sensors", address_);

  // parse buffer and update sensors
  Variant15Parser parser(buffer, true);
  parser.parse();

  if (part_number_ != nullptr) {
    part_number_->publish_state(parser.SAP_part_number);
  }

  if (serial_number_ != nullptr) {
    serial_number_->publish_state(parser.SAP_serial_number);
  }

  if (solar_voltage_ != nullptr) {
    solar_voltage_->publish_state(parser.Solar_voltage_input_1);
  }

  if (solar_current_ != nullptr) {
    solar_current_->publish_state(parser.Solar_current_input_1);
  }

  if (ac_current_ != nullptr) {
    ac_current_->publish_state(parser.AC_current);
  }

  if (ac_voltage_ != nullptr) {
    ac_voltage_->publish_state(parser.AC_voltage);
  }

  if (ac_power_ != nullptr) {
    ac_power_->publish_state(parser.AC_power);
  }

  if (ac_frequency_ != nullptr) {
    ac_frequency_->publish_state(parser.AC_frequency);
  }

  if (grid_ac_voltage_ != nullptr) {
    grid_ac_voltage_->publish_state(parser.AC_Grid_voltage);
  }

  if (grid_ac_frequency_ != nullptr) {
    grid_ac_frequency_->publish_state(parser.AC_Grid_frequency);
  }

  if (inverter_runtime_minutes_ != nullptr) {
    inverter_runtime_minutes_->publish_state(parser.Inverter_runtime_minutes);
  }

  if (day_supplied_ac_energy_ != nullptr) {
    day_supplied_ac_energy_->publish_state(parser.Day_supplied_ac_energy);
  }

  if (max_ac_power_today_ != nullptr) {
    max_ac_power_today_->publish_state(parser.Max_ac_power_today);
  }

  if (max_solar_input_power_ != nullptr) {
    max_solar_input_power_->publish_state(parser.Max_solar_1_input_power);
  }

  if (inverter_runtime_hours_ != nullptr) {
    inverter_runtime_hours_->publish_state(parser.Inverter_runtime_hours);
  }

  if (supplied_ac_energy_ != nullptr) {
    supplied_ac_energy_->publish_state(parser.Supplied_ac_energy);
  }

  // update timestamp
  last_update = millis();
}

}
}
