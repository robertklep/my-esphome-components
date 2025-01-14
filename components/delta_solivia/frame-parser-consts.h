#pragma once

// these should match the values in __init__.py
#define CONF_INV_PART_NUMBER               "part_number"
#define CONF_INV_SERIAL_NUMBER             "serial_number"

#define CONF_INV_SOLAR_VOLTAGE_PREFIX      "solar_voltage_input_"
#define CONF_INV_SOLAR_CURRENT_PREFIX      "solar_current_input_"
#define CONF_INV_SOLAR_POWER_PREFIX        "solar_power_input_"

#define CONF_INV_SOLAR_ISO_RES_INPUT_1     "solar_isolation_resistance_input_1"
#define CONF_INV_SOLAR_INPUT_MOV_RES       "solar_input_mov_resistance"

#define CONF_INV_TEMPERATURE_NTC_DC        "temperature_ntc_dc"
#define CONF_INV_TEMPERATURE_NTC_AC        "temperature_ntc_ac"

#define CONF_INV_AC_CURRENT                "ac_current"
#define CONF_INV_AC_VOLTAGE                "ac_voltage"
#define CONF_INV_AC_POWER                  "ac_power"
#define CONF_INV_AC_FREQ                   "ac_frequency"

#define CONF_INV_AC_CURRENT_PHASE_PREFIX   "ac_current_phase_"
#define CONF_INV_AC_VOLTAGE_PHASE_PREFIX   "ac_voltage_phase_"
#define CONF_INV_AC_POWER_PHASE_PREFIX     "ac_power_phase_"
#define CONF_INV_AC_FREQ_PHASE_PREFIX      "ac_frequency_phase_"

#define CONF_INV_SC_GRID_VOLTAGE           "sc_grid_voltage"
#define CONF_INV_SC_GRID_FREQUENCY         "sc_grid_frequency"
#define CONF_INV_SC_GRID_DC_INJ_CURRENT    "sc_grid_dc_injection_current"

#define CONF_INV_AC_GRID_VOLTAGE           "ac_grid_voltage"
#define CONF_INV_AC_GRID_FREQ              "ac_grid_frequency"
#define CONF_INV_AC_GRID_DC_INJ_CURRENT    "ac_grid_dc_injection_current"

#define CONF_INV_MAX_AC_CURRENT_TODAY      "max_ac_current_today"
#define CONF_INV_MIN_AC_VOLTAGE_TODAY      "min_ac_voltage_today"
#define CONF_INV_MAX_AC_VOLTAGE_TODAY      "max_ac_voltage_today"
#define CONF_INV_MAX_AC_POWER_TODAY        "max_ac_power_today"
#define CONF_INV_MIN_AC_FREQ_TODAY         "min_ac_frequency_today"
#define CONF_INV_MAX_AC_FREQ_TODAY         "max_ac_frequency_today"

#define CONF_INV_SUPPLIED_AC_ENERGY_TODAY  "supplied_ac_energy_today"
#define CONF_INV_SUPPLIED_AC_ENERGY_TOTAL  "supplied_ac_energy_total"

#define CONF_INV_RUNTIME_TOTAL             "runtime_total"
#define CONF_INV_RUNTIME_TODAY             "runtime_today"

#define CONF_INV_MAX_SOLAR_CURRENT_INPUT_1 "max_solar_current_input_1"
#define CONF_INV_MAX_SOLAR_VOLTAGE_INPUT_1 "max_solar_voltage_input_1"
#define CONF_INV_MAX_SOLAR_POWER_INPUT_1   "max_solar_power_input_1"

#define CONF_INV_MIN_SOLAR_ISO_RES_INPUT_1 "min_solar_isolation_resistance_input_1"
#define CONF_INV_MAX_SOLAR_ISO_RES_INPUT_1 "max_solar_isolation_resistance_input_1"
