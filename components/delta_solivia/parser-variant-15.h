#pragma once

#include <iostream>
#include "parser-base.h"

namespace esphome {
namespace delta_solivia {

// page 25
class Variant15Parser : public BaseParser {
  void parse_frame_(const uint8_t* frame, std::size_t pos) {
    publish_text_sensor_("SAP_part_number", parse_string(frame, pos, 11));
    pos += 11;
    publish_text_sensor_("SAP_serial_number", parse_string(frame, pos, 18));
    pos += 18;
    publish_sensor_("SAP_date_code", extract_int32(&frame[pos])); // Date Code);
    pos += 4;
    publish_sensor_("SAP_revision", extract_int16(&frame[pos])); // Date Code);
    pos += 2;

    // software revisions
    publish_sensor_("Software_rev_ac_major", frame[pos++]); // Major AC revision
    publish_sensor_("Software_rev_ac_minor", frame[pos++]); // Minor AC revision
    publish_sensor_("Software_rev_ac_bugfix", frame[pos++]); // AC bugfix
    publish_sensor_("Software_rev_dc_major", frame[pos++]); // Major DC revision
    publish_sensor_("Software_rev_dc_minor", frame[pos++]); // Minor DC revision
    publish_sensor_("Software_rev_dc_bugfix", frame[pos++]); // DC bugfix
    publish_sensor_("Software_rev_display_major", frame[pos++]); // Major Display revision
    publish_sensor_("Software_rev_display_minor", frame[pos++]); // Minor Display revision
    publish_sensor_("Software_rev_display_bugfix", frame[pos++]); // Display bugfix
    publish_sensor_("Software_rev_sc_major", frame[pos++]); // Major SC revision
    publish_sensor_("Software_rev_sc_minor", frame[pos++]); // Minor SC revision
    publish_sensor_("Software_rev_sc_bugfix", frame[pos++]); // SC bugfix

    // solar
    publish_sensor_("Solar_voltage_input_1", extract_int16(&frame[pos])); // Volts
    pos += 2;
    publish_sensor_("Solar_current_input_1", apply_scaling(extract_int16(&frame[pos]), 0.1)); // Amps
    pos += 2;
    publish_sensor_("Solar_isolation_resistance_input_1", extract_int16(&frame[pos])); // kOhms
    pos += 2;
    publish_sensor_("Temperature_ntc_dc", extract_int16(&frame[pos])); // Celsius
    pos += 2;
    publish_sensor_("Solar_input_MOV_resistance", extract_int16(&frame[pos])); // kOhms
    pos += 2;

    // AC
    publish_sensor_("AC_current", apply_scaling(extract_int16(&frame[pos]), 0.1)); // Amps
    pos += 2;
    publish_sensor_("AC_voltage", extract_int16(&frame[pos])); // Volts
    pos += 2;
    publish_sensor_("AC_power", extract_int16(&frame[pos])); // Watts
    pos += 2;
    publish_sensor_("AC_frequency", apply_scaling(extract_int16(&frame[pos]), 0.01)); // Hertz
    pos += 2;
    publish_sensor_("Temperature_ntc_ac", extract_int16(&frame[pos])); // Celsius
    pos += 2;

    // SC grid
    publish_sensor_("SC_Grid_voltage", apply_scaling(extract_int16(&frame[pos]), 0.01)); // Volts
    pos += 2;
    publish_sensor_("SC_Grid_frequency", apply_scaling(extract_int16(&frame[pos]), 0.01)); // Hertz
    pos += 2;
    publish_sensor_("SC_Grid_dc_injection_current", apply_scaling(extract_int16(&frame[pos]), 0.01)); // Amps
    pos += 2;

    // AC grid
    publish_sensor_("AC_Grid_voltage", apply_scaling(extract_int16(&frame[pos]), 0.01)); // Volts
    pos += 2;
    publish_sensor_("AC_Grid_frequency", apply_scaling(extract_int16(&frame[pos]), 0.01)); // Hertz
    pos += 2;
    publish_sensor_("AC_Grid_dc_injection_current", apply_scaling(extract_int16(&frame[pos]), 0.01)); // Amps
    pos += 2;

    // energy info
    publish_sensor_("Day_supplied_ac_energy", extract_int16(&frame[pos]) * 10); // Wh
    pos += 2;
    publish_sensor_("Inverter_runtime_minutes", extract_int16(&frame[pos])); // Minutes
    pos += 2;
    publish_sensor_("Max_ac_current_today", apply_scaling(extract_int16(&frame[pos]), 0.1)); // Amps
    pos += 2;
    publish_sensor_("Min_ac_voltage_today", extract_int16(&frame[pos])); // Volts
    pos += 2;
    publish_sensor_("Max_ac_voltage_today", extract_int16(&frame[pos])); // Volts
    pos += 2;
    publish_sensor_("Max_ac_power_today", extract_int16(&frame[pos])); // Watts
    pos += 2;
    publish_sensor_("Min_ac_frequency_today", apply_scaling(extract_int16(&frame[pos]), 0.01)); // Hertz
    pos += 2;
    publish_sensor_("Max_ac_frequency_today", apply_scaling(extract_int16(&frame[pos]), 0.01)); // Hertz
    pos += 2;
    publish_sensor_("Supplied_ac_energy", apply_scaling(extract_int32(&frame[pos]), 0.1)); // kWh
    pos += 4;
    publish_sensor_("Inverter_runtime_hours", extract_int32(&frame[pos])); // Hours
    pos += 4;

    // minimums/maximums
    publish_sensor_("Max_solar_1_input_current", apply_scaling(extract_int16(&frame[pos]), 0.1)); // Amps
    pos += 2;
    publish_sensor_("Max_solar_1_input_voltage", extract_int16(&frame[pos])); // Volts
    pos += 2;
    publish_sensor_("Max_solar_1_input_power", extract_int16(&frame[pos])); // Watts
    pos += 2;
    publish_sensor_("Min_isolation_resistance_solar_1", extract_int16(&frame[pos])); // kOhms
    pos += 2;
    publish_sensor_("Max_isolation_resistance_solar_1", extract_int16(&frame[pos])); // kOhms
    pos += 2;

    // limits and status
    publish_sensor_("Alarms_status", frame[pos++]); // Alarms status
    publish_sensor_("Status_dc_input", frame[pos++]); // Status DC input
    publish_sensor_("Limits_dc_input", frame[pos++]); // Limits DC input
    publish_sensor_("Status_ac_output", frame[pos++]); // Status AC output
    publish_sensor_("Limits_ac_output", frame[pos++]); // Limits AC output
    publish_sensor_("Warnings_status", frame[pos++]); // Warnings status
    publish_sensor_("DC_hardware_failure", frame[pos++]); // DC hardware failure
    publish_sensor_("AC_hardware_failure", frame[pos++]); // AC hardware failure
  }
public:
  //Variant15Parser(DeltaSoliviaInverter* inverter) : inverter_(inverter) {}
  Variant15Parser() {}
};

}
}
