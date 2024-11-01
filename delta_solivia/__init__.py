import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.cpp_helpers import gpio_pin_expression
from esphome.components import uart, sensor
from esphome.const import (
    CONF_ID,
    CONF_UART_ID,
    CONF_FLOW_CONTROL_PIN,
    DEVICE_CLASS_CURRENT,
    DEVICE_CLASS_ENERGY,
    DEVICE_CLASS_POWER,
    DEVICE_CLASS_VOLTAGE,
    DEVICE_CLASS_FREQUENCY,
    DEVICE_CLASS_DURATION,
    STATE_CLASS_MEASUREMENT,
    STATE_CLASS_TOTAL_INCREASING,
    UNIT_WATT,
    UNIT_KILOWATT,
    UNIT_KILOWATT_HOURS,
    UNIT_VOLT,
    UNIT_AMPERE,
    UNIT_HERTZ,
    UNIT_HOUR,
    UNIT_MINUTE,
)

CODEOWNERS   = ["@robertklep"]
DEPENDENCIES = ["uart"]
AUTO_LOAD    = ["sensor", "text_sensor"]

delta_solivia_ns      = cg.esphome_ns.namespace("delta_solivia")
DeltaSoliviaComponent = delta_solivia_ns.class_("DeltaSoliviaComponent", uart.UARTDevice, cg.PollingComponent)
DeltaSoliviaInverter  = delta_solivia_ns.class_("DeltaSoliviaInverter")

# global config
CONF_INVERTERS = "inverters"

# per-inverter config
CONF_INV_ADDRESS         = "address"
CONF_INV_UPDATE_INVERVAL = "update_interval"

# per-inverter measurements
CONF_INV_TOTAL_ENERGY          = "total_energy"
CONF_INV_TODAY_ENERGY          = "today_energy"
CONF_INV_DC_VOLTAGE            = "dc_voltage"
CONF_INV_DC_CURRENT            = "dc_current"
CONF_INV_AC_VOLTAGE            = "ac_voltage"
CONF_INV_AC_CURRENT            = "ac_current"
CONF_INV_AC_FREQ               = "ac_frequency"
CONF_INV_AC_POWER              = "ac_power"
CONF_INV_GRID_VOLTAGE          = "grid_voltage"
CONF_INV_GRID_FREQ             = "grid_frequency"
CONF_INV_RUNTIME_HOURS         = "runtime_hours"
CONF_INV_RUNTIME_MINUTES       = "runtime_minutes"
CONF_INV_MAX_AC_POWER          = "max_ac_power_today"
CONF_INV_MAX_SOLAR_INPUT_POWER = "max_solar_input_power"

def _validate_inverters(config):
    if len(config) < 1:
        raise cv.Invalid("Need at least one inverter to be configured")
    # ensure all configured inverters have a unique address
    addresses = { inverter.get(CONF_INV_ADDRESS) for inverter in config }
    if len(addresses) != len(config):
        raise cv.Invalid("Inverter addresses should be unique")
    return config

INVERTER_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(DeltaSoliviaInverter),
    cv.Required(CONF_INV_ADDRESS): cv.int_range(min = 1),
    cv.Optional(CONF_INV_UPDATE_INVERVAL): cv.update_interval,
    cv.Optional(CONF_INV_TOTAL_ENERGY): sensor.sensor_schema(
        unit_of_measurement = UNIT_KILOWATT_HOURS,
        icon                = 'mdi:meter-electric',
        accuracy_decimals   = 0,
        device_class        = DEVICE_CLASS_ENERGY,
        state_class         = STATE_CLASS_TOTAL_INCREASING
    ),
    cv.Optional(CONF_INV_TODAY_ENERGY): sensor.sensor_schema(
        unit_of_measurement = UNIT_KILOWATT_HOURS,
        icon                = 'mdi:meter-electric',
        accuracy_decimals   = 0,
        device_class        = DEVICE_CLASS_ENERGY,
        state_class         = STATE_CLASS_TOTAL_INCREASING
    ),
    cv.Optional(CONF_INV_DC_VOLTAGE): sensor.sensor_schema(
        unit_of_measurement = UNIT_VOLT,
        accuracy_decimals   = 0,
        device_class        = DEVICE_CLASS_VOLTAGE,
        state_class         = STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_INV_DC_CURRENT): sensor.sensor_schema(
        unit_of_measurement = UNIT_AMPERE,
        accuracy_decimals   = 1,
        device_class        = DEVICE_CLASS_CURRENT,
        state_class         = STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_INV_AC_VOLTAGE): sensor.sensor_schema(
        unit_of_measurement = UNIT_VOLT,
        accuracy_decimals   = 0,
        device_class        = DEVICE_CLASS_VOLTAGE,
        state_class         = STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_INV_AC_CURRENT): sensor.sensor_schema(
        unit_of_measurement = UNIT_AMPERE,
        accuracy_decimals   = 1,
        device_class        = DEVICE_CLASS_CURRENT,
        state_class         = STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_INV_AC_FREQ): sensor.sensor_schema(
        unit_of_measurement = UNIT_HERTZ,
        accuracy_decimals   = 2,
        device_class        = DEVICE_CLASS_FREQUENCY,
        state_class         = STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_INV_AC_POWER): sensor.sensor_schema(
        unit_of_measurement = UNIT_WATT,
        icon                = 'mdi:solar-power',
        accuracy_decimals   = 0,
        device_class        = DEVICE_CLASS_POWER,
        state_class         = STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_INV_GRID_VOLTAGE): sensor.sensor_schema(
        unit_of_measurement = UNIT_VOLT,
        accuracy_decimals   = 0,
        device_class        = DEVICE_CLASS_VOLTAGE,
        state_class         = STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_INV_GRID_FREQ): sensor.sensor_schema(
        unit_of_measurement = UNIT_HERTZ,
        accuracy_decimals   = 2,
        device_class        = DEVICE_CLASS_FREQUENCY,
        state_class         = STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_INV_RUNTIME_HOURS): sensor.sensor_schema(
        unit_of_measurement = UNIT_HOUR,
        accuracy_decimals   = 0,
        device_class        = DEVICE_CLASS_DURATION,
        state_class         = STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_INV_RUNTIME_MINUTES): sensor.sensor_schema(
        unit_of_measurement = UNIT_MINUTE,
        accuracy_decimals   = 0,
        device_class        = DEVICE_CLASS_DURATION,
        state_class         = STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_INV_MAX_AC_POWER): sensor.sensor_schema(
        unit_of_measurement = UNIT_WATT,
        icon                = 'mdi:solar-power',
        accuracy_decimals   = 0,
        device_class        = DEVICE_CLASS_POWER,
        state_class         = STATE_CLASS_TOTAL_INCREASING
    ),
    cv.Optional(CONF_INV_MAX_SOLAR_INPUT_POWER): sensor.sensor_schema(
        unit_of_measurement = UNIT_WATT,
        icon                = 'mdi:solar-power',
        accuracy_decimals   = 0,
        device_class        = DEVICE_CLASS_POWER,
        state_class         = STATE_CLASS_TOTAL_INCREASING
    ),
})

CONFIG_SCHEMA = cv.All(
    cv.Schema({
        cv.GenerateID(): cv.declare_id(DeltaSoliviaComponent),
        cv.Optional(CONF_FLOW_CONTROL_PIN): pins.gpio_output_pin_schema,
        cv.Required(CONF_INVERTERS): cv.All(cv.ensure_list(INVERTER_SCHEMA), _validate_inverters),
    })
    .extend(cv.polling_component_schema("1s"))
    .extend(uart.UART_DEVICE_SCHEMA)
)

async def to_code(config):
    component = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(component , config)
    await uart.register_uart_device(component , config)

    if CONF_FLOW_CONTROL_PIN in config:
        pin = await gpio_pin_expression(config[CONF_FLOW_CONTROL_PIN])
        cg.add(component.set_flow_control_pin(pin))

    for inverter_config in config[CONF_INVERTERS]:
        inverter = cg.new_Pvariable(inverter_config[CONF_ID], DeltaSoliviaInverter(inverter_config[CONF_INV_ADDRESS]))

        cg.add(inverter.set_update_interval(inverter_config[CONF_INV_UPDATE_INVERVAL]))

        if CONF_INV_TOTAL_ENERGY in inverter_config:
            sens = await sensor.new_sensor(inverter_config[CONF_INV_TOTAL_ENERGY])
            cg.add(inverter.set_supplied_ac_energy(sens))

        if CONF_INV_TODAY_ENERGY in inverter_config:
            sens = await sensor.new_sensor(inverter_config[CONF_INV_TODAY_ENERGY])
            cg.add(inverter.set_day_supplied_ac_energy(sens))

        if CONF_INV_DC_VOLTAGE in inverter_config:
            sens = await sensor.new_sensor(inverter_config[CONF_INV_DC_VOLTAGE])
            cg.add(inverter.set_solar_voltage(sens))

        if CONF_INV_DC_CURRENT in inverter_config:
            sens = await sensor.new_sensor(inverter_config[CONF_INV_DC_CURRENT])
            cg.add(inverter.set_solar_current(sens))

        if CONF_INV_AC_VOLTAGE in inverter_config:
            sens = await sensor.new_sensor(inverter_config[CONF_INV_AC_VOLTAGE])
            cg.add(inverter.set_ac_voltage(sens))

        if CONF_INV_AC_CURRENT in inverter_config:
            sens = await sensor.new_sensor(inverter_config[CONF_INV_AC_CURRENT])
            cg.add(inverter.set_ac_current(sens))

        if CONF_INV_AC_FREQ in inverter_config:
            sens = await sensor.new_sensor(inverter_config[CONF_INV_AC_FREQ])
            cg.add(inverter.set_ac_frequency(sens))

        if CONF_INV_AC_POWER in inverter_config:
            sens = await sensor.new_sensor(inverter_config[CONF_INV_AC_POWER])
            cg.add(inverter.set_ac_power(sens))

        if CONF_INV_GRID_VOLTAGE in inverter_config:
            sens = await sensor.new_sensor(inverter_config[CONF_INV_GRID_VOLTAGE])
            cg.add(inverter.set_grid_ac_voltage(sens))

        if CONF_INV_GRID_FREQ in inverter_config:
            sens = await sensor.new_sensor(inverter_config[CONF_INV_GRID_FREQ])
            cg.add(inverter.set_grid_ac_frequency(sens))

        if CONF_INV_RUNTIME_HOURS in inverter_config:
            sens = await sensor.new_sensor(inverter_config[CONF_INV_RUNTIME_HOURS])
            cg.add(inverter.set_inverter_runtime_hours(sens))

        if CONF_INV_RUNTIME_MINUTES in inverter_config:
            sens = await sensor.new_sensor(inverter_config[CONF_INV_RUNTIME_MINUTES])
            cg.add(inverter.set_inverter_runtime_minutes(sens))

        if CONF_INV_MAX_AC_POWER in inverter_config:
            sens = await sensor.new_sensor(inverter_config[CONF_INV_MAX_AC_POWER])
            cg.add(inverter.set_max_ac_power_today(sens))

        if CONF_INV_MAX_SOLAR_INPUT_POWER in inverter_config:
            sens = await sensor.new_sensor(inverter_config[CONF_INV_MAX_SOLAR_INPUT_POWER])
            cg.add(inverter.set_max_solar_input_power(sens))

        cg.add(component .add_inverter(inverter))

