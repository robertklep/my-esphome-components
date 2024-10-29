#pragma once

#include <string>
#include <vector>
#include <cstdint>

// Utility functions for byte extraction and scaling
static int16_t extractInt16(const uint8_t *data) {
  return (data[0] << 8) | data[1];
}

static uint32_t extractInt32(const uint8_t *data) {
  return (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
}

static float applyScaling(int16_t value, float scale) {
  return value * scale;
}

static float applyScaling(uint32_t value, float scale) {
  return value * scale;
}

class Variant15Parser {
public:
  // Data fields
  std::string SAP_part_number;
  std::string SAP_serial_number;
  int32_t SAP_date_code; // Date Code
  int16_t SAP_revision;  // Revision
  int8_t Software_rev_ac_major, Software_rev_ac_minor, Software_rev_ac_bugfix;
  int8_t Software_rev_dc_major, Software_rev_dc_minor, Software_rev_dc_bugfix;
  int8_t Software_rev_display_major, Software_rev_display_minor, Software_rev_display_bugfix;
  int8_t Software_rev_sc_major, Software_rev_sc_minor, Software_rev_sc_bugfix;
  int16_t Solar_voltage_input_1; // Volts
  float Solar_current_input_1;   // Amps
  int16_t Solar_isolation_resistance_input_1; // kOhms
  int16_t Temperature_ntc_dc; // Celsius
  int16_t Solar_input_MOV_resistance; // kOhms
  float AC_current; // Amps
  int16_t AC_voltage; // Volts
  int16_t AC_power; // Watts
  float AC_frequency; // Hertz
  int16_t Temperature_ntc_ac; // Celsius
  float SC_Grid_voltage; // Volts
  float SC_Grid_frequency; // Hertz
  float SC_Grid_dc_injection_current; // Amps
  float AC_Grid_voltage; // Volts
  float AC_Grid_frequency; // Hertz
  float AC_Grid_dc_injection_current; // Amps
  int16_t Day_supplied_ac_energy; // Wh
  int16_t Inverter_runtime_minutes; // Minutes
  float Max_ac_current_today; // Amps
  int16_t Min_ac_voltage_today; // Volts
  int16_t Max_ac_voltage_today; // Volts
  int16_t Max_ac_power_today; // Watts
  float Min_ac_frequency_today; // Hertz
  float Max_ac_frequency_today; // Hertz
  float Supplied_ac_energy; // kWh
  uint32_t Inverter_runtime_hours; // Hours
  float Max_solar_1_input_current; // Amps
  int16_t Max_solar_1_input_voltage; // Volts
  int16_t Max_solar_1_input_power; // Watts
  int16_t Min_isolation_resistance_solar_1; // kOhms
  int16_t Max_isolation_resistance_solar_1; // kOhms
  uint8_t Alarms_status;
  uint8_t Status_dc_input;
  uint8_t Limits_dc_input;
  uint8_t Status_ac_output;
  uint8_t Limits_ac_output;
  uint8_t Warnings_status;
  uint8_t DC_hardware_failure;
  uint8_t AC_hardware_failure;

  // ctor
  Variant15Parser (const std::vector<uint8_t>& data, bool skipHeader = false) : data(data), pos(skipHeader ? 6 : 0) {}

  void parse() {
    parseSAPInfo();
    parseSoftwareRevisions();
    parseSolarInputs();
    parseACInputs();
    parseSCGridInfo();
    parseACGridInfo();
    parseEnergyInfo();
    parseLimitsAndStatus();
  }

private:
  std::vector<uint8_t> data;
  size_t pos;

  void parseSAPInfo() {
    SAP_part_number = parseString(11);
    SAP_serial_number = parseString(18);
    SAP_date_code = extractInt32(&data[pos]); // Date Code
    pos += 4;
    SAP_revision = extractInt16(&data[pos]); // Revision
    pos += 2;
  }

  void parseSoftwareRevisions() {
    Software_rev_ac_major = data[pos++]; // Major AC revision
    Software_rev_ac_minor = data[pos++]; // Minor AC revision
    Software_rev_ac_bugfix = data[pos++]; // AC bugfix
    Software_rev_dc_major = data[pos++]; // Major DC revision
    Software_rev_dc_minor = data[pos++]; // Minor DC revision
    Software_rev_dc_bugfix = data[pos++]; // DC bugfix
    Software_rev_display_major = data[pos++]; // Major Display revision
    Software_rev_display_minor = data[pos++]; // Minor Display revision
    Software_rev_display_bugfix = data[pos++]; // Display bugfix
    Software_rev_sc_major = data[pos++]; // Major SC revision
    Software_rev_sc_minor = data[pos++]; // Minor SC revision
    Software_rev_sc_bugfix = data[pos++]; // SC bugfix
  }

  void parseSolarInputs() {
    Solar_voltage_input_1 = extractInt16(&data[pos]); // Volts
    pos += 2;
    Solar_current_input_1 = applyScaling(extractInt16(&data[pos]), 0.1); // Amps
    pos += 2;
    Solar_isolation_resistance_input_1 = extractInt16(&data[pos]); // kOhms
    pos += 2;
    Temperature_ntc_dc = extractInt16(&data[pos]); // Celsius
    pos += 2;
    Solar_input_MOV_resistance = extractInt16(&data[pos]); // kOhms
    pos += 2;
  }

  void parseACInputs() {
    AC_current = applyScaling(extractInt16(&data[pos]), 0.1); // Amps
    pos += 2;
    AC_voltage = extractInt16(&data[pos]); // Volts
    pos += 2;
    AC_power = extractInt16(&data[pos]); // Watts
    pos += 2;
    AC_frequency = applyScaling(extractInt16(&data[pos]), 0.01); // Hertz
    pos += 2;
    Temperature_ntc_ac = extractInt16(&data[pos]); // Celsius
    pos += 2;
  }

  void parseSCGridInfo() {
    SC_Grid_voltage = applyScaling(extractInt16(&data[pos]), 0.01); // Volts
    pos += 2;
    SC_Grid_frequency = applyScaling(extractInt16(&data[pos]), 0.01); // Hertz
    pos += 2;
    SC_Grid_dc_injection_current = applyScaling(extractInt16(&data[pos]), 0.01); // Amps
    pos += 2;
  }

  void parseACGridInfo() {
    AC_Grid_voltage = applyScaling(extractInt16(&data[pos]), 0.01); // Volts
    pos += 2;
    AC_Grid_frequency = applyScaling(extractInt16(&data[pos]), 0.01); // Hertz
    pos += 2;
    AC_Grid_dc_injection_current = applyScaling(extractInt16(&data[pos]), 0.01); // Amps
    pos += 2;
  }

  void parseEnergyInfo() {
    Day_supplied_ac_energy = extractInt16(&data[pos]) * 10; // Wh
    pos += 2;
    Inverter_runtime_minutes = extractInt16(&data[pos]); // Minutes
    pos += 2;
    Max_ac_current_today = applyScaling(extractInt16(&data[pos]), 0.1); // Amps
    pos += 2;
    Min_ac_voltage_today = extractInt16(&data[pos]); // Volts
    pos += 2;
    Max_ac_voltage_today = extractInt16(&data[pos]); // Volts
    pos += 2;
    Max_ac_power_today = extractInt16(&data[pos]); // Watts
    pos += 2;
    Min_ac_frequency_today = applyScaling(extractInt16(&data[pos]), 0.01); // Hertz
    pos += 2;
    Max_ac_frequency_today = applyScaling(extractInt16(&data[pos]), 0.01); // Hertz
    pos += 2;
    Supplied_ac_energy = applyScaling(extractInt32(&data[pos]), 0.1); // kWh
    pos += 4;
    Inverter_runtime_hours = extractInt32(&data[pos]); // Hours
    pos += 4;
  }

  void parseLimitsAndStatus() {
    Max_solar_1_input_current = applyScaling(extractInt16(&data[pos]), 0.1); // Amps
    pos += 2;
    Max_solar_1_input_voltage = extractInt16(&data[pos]); // Volts
    pos += 2;
    Max_solar_1_input_power = extractInt16(&data[pos]); // Watts
    pos += 2;
    Min_isolation_resistance_solar_1 = extractInt16(&data[pos]); // kOhms
    pos += 2;
    Max_isolation_resistance_solar_1 = extractInt16(&data[pos]); // kOhms
    pos += 2;
    Alarms_status = data[pos++]; // Alarms status
    Status_dc_input = data[pos++]; // Status DC input
    Limits_dc_input = data[pos++]; // Limits DC input
    Status_ac_output = data[pos++]; // Status AC output
    Limits_ac_output = data[pos++]; // Limits AC output
    Warnings_status = data[pos++]; // Warnings status
    DC_hardware_failure = data[pos++]; // DC hardware failure
    AC_hardware_failure = data[pos++]; // AC hardware failure
  }

  std::string parseString(int length) {
    std::string result(data.begin() + pos, data.begin() + pos + length);
    pos += length;
    return result;
  }
};
