#pragma once

#include <iostream>
#include "parser-base.h"

namespace esphome {
namespace delta_solivia {

// page 25
class Variant15Parser : public BaseParser {
  void parseFrame_(const uint8_t* frame, std::size_t pos) {
    publishTextSensor_("SAP_part_number", parseString(frame, pos, 11));
    pos += 11;
    publishTextSensor_("SAP_serial_number", parseString(frame, pos, 18));
    pos += 18;
    publishSensor_("SAP_date_code", extractInt32(&frame[pos])); // Date Code);
    pos += 4;
    publishSensor_("SAP_revision", extractInt16(&frame[pos])); // Date Code);
    pos += 2;

    // software revisions
    publishSensor_("Software_rev_ac_major", frame[pos++]); // Major AC revision
    publishSensor_("Software_rev_ac_minor", frame[pos++]); // Minor AC revision
    publishSensor_("Software_rev_ac_bugfix", frame[pos++]); // AC bugfix
    publishSensor_("Software_rev_dc_major", frame[pos++]); // Major DC revision
    publishSensor_("Software_rev_dc_minor", frame[pos++]); // Minor DC revision
    publishSensor_("Software_rev_dc_bugfix", frame[pos++]); // DC bugfix
    publishSensor_("Software_rev_display_major", frame[pos++]); // Major Display revision
    publishSensor_("Software_rev_display_minor", frame[pos++]); // Minor Display revision
    publishSensor_("Software_rev_display_bugfix", frame[pos++]); // Display bugfix
    publishSensor_("Software_rev_sc_major", frame[pos++]); // Major SC revision
    publishSensor_("Software_rev_sc_minor", frame[pos++]); // Minor SC revision
    publishSensor_("Software_rev_sc_bugfix", frame[pos++]); // SC bugfix

    // solar
    publishSensor_("Solar_voltage_input_1", extractInt16(&frame[pos])); // Volts
    pos += 2;
    publishSensor_("Solar_current_input_1", applyScaling(extractInt16(&frame[pos]), 0.1)); // Amps
    pos += 2;
    publishSensor_("Solar_isolation_resistance_input_1", extractInt16(&frame[pos])); // kOhms
    pos += 2;
    publishSensor_("Temperature_ntc_dc", extractInt16(&frame[pos])); // Celsius
    pos += 2;
    publishSensor_("Solar_input_MOV_resistance", extractInt16(&frame[pos])); // kOhms
    pos += 2;

    // AC
    publishSensor_("AC_current", applyScaling(extractInt16(&frame[pos]), 0.1)); // Amps
    pos += 2;
    publishSensor_("AC_voltage", extractInt16(&frame[pos])); // Volts
    pos += 2;
    publishSensor_("AC_power", extractInt16(&frame[pos])); // Watts
    pos += 2;
    publishSensor_("AC_frequency", applyScaling(extractInt16(&frame[pos]), 0.01)); // Hertz
    pos += 2;
    publishSensor_("Temperature_ntc_ac", extractInt16(&frame[pos])); // Celsius
    pos += 2;

    // SC grid
    publishSensor_("SC_Grid_voltage", applyScaling(extractInt16(&frame[pos]), 0.01)); // Volts
    pos += 2;
    publishSensor_("SC_Grid_frequency", applyScaling(extractInt16(&frame[pos]), 0.01)); // Hertz
    pos += 2;
    publishSensor_("SC_Grid_dc_injection_current", applyScaling(extractInt16(&frame[pos]), 0.01)); // Amps
    pos += 2;

    // AC grid
    publishSensor_("AC_Grid_voltage", applyScaling(extractInt16(&frame[pos]), 0.01)); // Volts
    pos += 2;
    publishSensor_("AC_Grid_frequency", applyScaling(extractInt16(&frame[pos]), 0.01)); // Hertz
    pos += 2;
    publishSensor_("AC_Grid_dc_injection_current", applyScaling(extractInt16(&frame[pos]), 0.01)); // Amps
    pos += 2;

    // energy info
    publishSensor_("Day_supplied_ac_energy", extractInt16(&frame[pos]) * 10); // Wh
    pos += 2;
    publishSensor_("Inverter_runtime_minutes", extractInt16(&frame[pos])); // Minutes
    pos += 2;
    publishSensor_("Max_ac_current_today", applyScaling(extractInt16(&frame[pos]), 0.1)); // Amps
    pos += 2;
    publishSensor_("Min_ac_voltage_today", extractInt16(&frame[pos])); // Volts
    pos += 2;
    publishSensor_("Max_ac_voltage_today", extractInt16(&frame[pos])); // Volts
    pos += 2;
    publishSensor_("Max_ac_power_today", extractInt16(&frame[pos])); // Watts
    pos += 2;
    publishSensor_("Min_ac_frequency_today", applyScaling(extractInt16(&frame[pos]), 0.01)); // Hertz
    pos += 2;
    publishSensor_("Max_ac_frequency_today", applyScaling(extractInt16(&frame[pos]), 0.01)); // Hertz
    pos += 2;
    publishSensor_("Supplied_ac_energy", applyScaling(extractInt32(&frame[pos]), 0.1)); // kWh
    pos += 4;
    publishSensor_("Inverter_runtime_hours", extractInt32(&frame[pos])); // Hours
    pos += 4;

    // minimums/maximums
    publishSensor_("Max_solar_1_input_current", applyScaling(extractInt16(&frame[pos]), 0.1)); // Amps
    pos += 2;
    publishSensor_("Max_solar_1_input_voltage", extractInt16(&frame[pos])); // Volts
    pos += 2;
    publishSensor_("Max_solar_1_input_power", extractInt16(&frame[pos])); // Watts
    pos += 2;
    publishSensor_("Min_isolation_resistance_solar_1", extractInt16(&frame[pos])); // kOhms
    pos += 2;
    publishSensor_("Max_isolation_resistance_solar_1", extractInt16(&frame[pos])); // kOhms
    pos += 2;

    // limits and status
    publishSensor_("Alarms_status", frame[pos++]); // Alarms status
    publishSensor_("Status_dc_input", frame[pos++]); // Status DC input
    publishSensor_("Limits_dc_input", frame[pos++]); // Limits DC input
    publishSensor_("Status_ac_output", frame[pos++]); // Status AC output
    publishSensor_("Limits_ac_output", frame[pos++]); // Limits AC output
    publishSensor_("Warnings_status", frame[pos++]); // Warnings status
    publishSensor_("DC_hardware_failure", frame[pos++]); // DC hardware failure
    publishSensor_("AC_hardware_failure", frame[pos++]); // AC hardware failure
  }
public:
  //Variant15Parser(DeltaSoliviaInverter* inverter) : inverter_(inverter) {}
  Variant15Parser() {}
};

}
}
