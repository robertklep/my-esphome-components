#pragma once

// these should match the values in __init__.py
inline const char* CONF_INV_PART_NUMBER               = "part_number";
inline const char* CONF_INV_SERIAL_NUMBER             = "serial_number";

inline const char* CONF_INV_SOLAR_VOLTAGE_INPUT_1     = "solar_voltage_input_1";
inline const char* CONF_INV_SOLAR_CURRENT_INPUT_1     = "solar_current_input_1";
inline const char* CONF_INV_SOLAR_ISO_RES_INPUT_1     = "solar_isolation_resistance_input_1";
inline const char* CONF_INV_SOLAR_INPUT_MOV_RES       = "solar_input_mov_resistance";

inline const char* CONF_INV_TEMPERATURE_NTC_DC        = "temperature_ntc_dc";
inline const char* CONF_INV_TEMPERATURE_NTC_AC        = "temperature_ntc_ac";

inline const char* CONF_INV_AC_CURRENT                = "ac_current";
inline const char* CONF_INV_AC_VOLTAGE                = "ac_voltage";
inline const char* CONF_INV_AC_POWER                  = "ac_power";
inline const char* CONF_INV_AC_FREQ                   = "ac_frequency";

inline const char* CONF_INV_SC_GRID_VOLTAGE           = "sc_grid_voltage";
inline const char* CONF_INV_SC_GRID_FREQUENCY         = "sc_grid_frequency";
inline const char* CONF_INV_SC_GRID_DC_INJ_CURRENT    = "sc_grid_dc_injection_current";

inline const char* CONF_INV_AC_GRID_VOLTAGE           = "ac_grid_voltage";
inline const char* CONF_INV_AC_GRID_FREQ              = "ac_grid_frequency";
inline const char* CONF_INV_AC_GRID_DC_INJ_CURRENT    = "ac_grid_dc_injection_current";

inline const char* CONF_INV_MAX_AC_CURRENT_TODAY      = "max_ac_current_today";
inline const char* CONF_INV_MIN_AC_VOLTAGE_TODAY      = "min_ac_voltage_today";
inline const char* CONF_INV_MAX_AC_VOLTAGE_TODAY      = "max_ac_voltage_today";
inline const char* CONF_INV_MAX_AC_POWER_TODAY        = "max_ac_power_today";
inline const char* CONF_INV_MIN_AC_FREQ_TODAY         = "min_ac_frequency_today";
inline const char* CONF_INV_MAX_AC_FREQ_TODAY         = "max_ac_frequency_today";

inline const char* CONF_INV_SUPPLIED_AC_ENERGY_TODAY  = "supplied_ac_energy_today";
inline const char* CONF_INV_SUPPLIED_AC_ENERGY_TOTAL  = "supplied_ac_energy_total";

inline const char* CONF_INV_RUNTIME_TOTAL             = "runtime_total";
inline const char* CONF_INV_RUNTIME_TODAY             = "runtime_today";

inline const char* CONF_INV_MAX_SOLAR_CURRENT_INPUT_1 = "max_solar_current_input_1";
inline const char* CONF_INV_MAX_SOLAR_VOLTAGE_INPUT_1 = "max_solar_voltage_input_1";
inline const char* CONF_INV_MAX_SOLAR_POWER_INPUT_1   = "max_solar_power_input_1";

inline const char* CONF_INV_MIN_SOLAR_ISO_RES_INPUT_1 = "min_solar_isolation_resistance_input_1";
inline const char* CONF_INV_MAX_SOLAR_ISO_RES_INPUT_1 = "max_solar_isolation_resistance_input_1";
