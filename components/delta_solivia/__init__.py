import logging
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.cpp_helpers import gpio_pin_expression
from esphome.components import uart, sensor, text_sensor
from esphome.const import (
    CONF_ID,
    CONF_UART_ID,
    CONF_UPDATE_INTERVAL,
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
    UNIT_WATT_HOURS,
    UNIT_KILOWATT,
    UNIT_KILOWATT_HOURS,
    UNIT_VOLT,
    UNIT_AMPERE,
    UNIT_HERTZ,
    UNIT_SECOND,
)

LOGGER = logging.getLogger(__name__)

CODEOWNERS   = ["@robertklep"]
DEPENDENCIES = ["uart"]
AUTO_LOAD    = ["sensor", "text_sensor"]

delta_solivia_ns      = cg.esphome_ns.namespace("delta_solivia")
DeltaSoliviaComponent = delta_solivia_ns.class_("DeltaSoliviaComponent", uart.UARTDevice, cg.PollingComponent)
DeltaSoliviaInverter  = delta_solivia_ns.class_("DeltaSoliviaInverter")

# global config
CONF_INVERTERS = "inverters"
CONF_HAS_GATEWAY = "has_gateway"

# per-inverter config
CONF_INV_ADDRESS  = "address"
CONF_INV_VARIANT = "variant"
CONF_INV_THROTTLE = "throttle"

# per-inverter measurements
CONF_INV_PART_NUMBER           = "part_number"
CONF_INV_SERIAL_NUMBER         = "serial_number"
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
CONF_INV_RUNTIME_TOTAL         = "runtime_total"
CONF_INV_RUNTIME_TODAY         = "runtime_today"
CONF_INV_MAX_AC_POWER          = "max_ac_power_today"
CONF_INV_MAX_SOLAR_INPUT_POWER = "max_solar_input_power"

def _parser_for_variant(variant):
    if variant in [ 15, 18, 19, 20, 31, 34, 35, 36, 38, 39, 55, 58, 59, 60 ]:
        return 15
    elif variant >= 212 and variant <= 222:
        return 212
    else:
        return None

def _validate_inverters(config):
    if len(config) < 1:
        raise cv.Invalid("Need at least one inverter to be configured")
    # ensure all configured inverters have a unique address
    addresses = { inverter.get(CONF_INV_ADDRESS) for inverter in config }
    if len(addresses) != len(config):
        raise cv.Invalid("Inverter addresses should be unique")
    # check if variants are supported
    variants = { inverter.get(CONF_INV_VARIANT) for inverter in config }
    for variant in variants:
        parser = _parser_for_variant(variant)
        if parser is None:
            raise cv.Invalid(f"Variant {variant} not supported")
    return config

INVERTER_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(DeltaSoliviaInverter),
    cv.Required(CONF_INV_ADDRESS): cv.int_range(min = 1),
    cv.Optional(CONF_INV_VARIANT, default = 15): cv.int_range(min = 1, max = 222),
    cv.Optional(CONF_INV_THROTTLE, default = '10s'): cv.update_interval,
    cv.Optional(CONF_INV_PART_NUMBER): text_sensor.text_sensor_schema(),
    cv.Optional(CONF_INV_SERIAL_NUMBER): text_sensor.text_sensor_schema(),
    cv.Optional(CONF_INV_TOTAL_ENERGY): sensor.sensor_schema(
        unit_of_measurement = UNIT_KILOWATT_HOURS,
        icon                = 'mdi:meter-electric',
        accuracy_decimals   = 0,
        device_class        = DEVICE_CLASS_ENERGY,
        state_class         = STATE_CLASS_TOTAL_INCREASING
    ),
    cv.Optional(CONF_INV_TODAY_ENERGY): sensor.sensor_schema(
        unit_of_measurement = UNIT_WATT_HOURS,
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
    cv.Optional(CONF_INV_RUNTIME_TOTAL): sensor.sensor_schema(
        unit_of_measurement = UNIT_SECOND,
        accuracy_decimals   = 0,
        device_class        = DEVICE_CLASS_DURATION,
        state_class         = STATE_CLASS_TOTAL_INCREASING,
    ),
    cv.Optional(CONF_INV_RUNTIME_TODAY): sensor.sensor_schema(
        unit_of_measurement = UNIT_SECOND,
        accuracy_decimals   = 0,
        device_class        = DEVICE_CLASS_DURATION,
        state_class         = STATE_CLASS_TOTAL_INCREASING,
    ),
    cv.Optional(CONF_INV_MAX_AC_POWER): sensor.sensor_schema(
        unit_of_measurement = UNIT_WATT,
        icon                = 'mdi:solar-power',
        accuracy_decimals   = 0,
        device_class        = DEVICE_CLASS_POWER,
        state_class         = STATE_CLASS_MEASUREMENT
    ),
    cv.Optional(CONF_INV_MAX_SOLAR_INPUT_POWER): sensor.sensor_schema(
        unit_of_measurement = UNIT_WATT,
        icon                = 'mdi:solar-power',
        accuracy_decimals   = 0,
        device_class        = DEVICE_CLASS_POWER,
        state_class         = STATE_CLASS_MEASUREMENT
    ),
})

CONFIG_SCHEMA = cv.All(
    cv.Schema({
        cv.GenerateID(): cv.declare_id(DeltaSoliviaComponent),
        cv.Optional(CONF_FLOW_CONTROL_PIN): pins.gpio_output_pin_schema,
        cv.Optional(CONF_HAS_GATEWAY, default = False): cv.boolean,
        cv.Required(CONF_INVERTERS): cv.All(cv.ensure_list(INVERTER_SCHEMA), _validate_inverters),
    })
    .extend(cv.polling_component_schema("5s"))
    .extend(uart.UART_DEVICE_SCHEMA)
)

async def to_code(config):
    component = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(component , config)
    await uart.register_uart_device(component , config)

    if CONF_FLOW_CONTROL_PIN in config:
        pin = await gpio_pin_expression(config[CONF_FLOW_CONTROL_PIN])
        cg.add(component.set_flow_control_pin(pin))

    # update interval for component should be different depending on
    # whether there's a gateway present or not (the gateway will request
    # updates often, and the polling interval needs to be a lot shorter)
    has_gateway     = config[CONF_HAS_GATEWAY]
    update_interval = config[CONF_UPDATE_INTERVAL].total_milliseconds
    if not has_gateway and update_interval < 1000:
        LOGGER.warning("— [Solivia] Component update interval for non-gateway operation shouldn't be lower than 1000ms (is now set to %ums)", update_interval)
    elif has_gateway and update_interval != 500:
        LOGGER.warning("— [Solivia] Fixing component update interval to 500ms")
        cg.add(component.set_update_interval(500))
    cg.add(component.set_has_gateway(has_gateway))

    for inverter_config in config[CONF_INVERTERS]:
        address  = inverter_config[CONF_INV_ADDRESS]
        variant = inverter_config[CONF_INV_VARIANT]
        throttle = inverter_config[CONF_INV_THROTTLE];
        inverter = cg.new_Pvariable(inverter_config[CONF_ID], DeltaSoliviaInverter(address, variant))

        # set throttle interval on component, which is used
        # to prevent excessive work when running in gateway mode
        cg.add(component.set_throttle(throttle));

        # create all numerical sensors, each one with a throttle_average filter
        # to prevent overloading HA
        async def make_sensor(field):
            # create the throttle filter
            filter_id = cv.declare_id(sensor.ThrottleAverageFilter)(f'{field}_{address}')
            filter = cg.new_Pvariable(filter_id, sensor.ThrottleAverageFilter(throttle))
            cg.add(cg.App.register_component(filter))
            cg.add(filter.set_component_source('sensor'))

            sens = await sensor.new_sensor(inverter_config[field])
            cg.add(sens.add_filters([ filter ]))

            cg.add(inverter.add_sensor(field, sens))

        async def make_text_sensor(field):
            sens = await text_sensor.new_text_sensor(inverter_config[field])
            cg.add(inverter.add_text_sensor(field, sens))

        # add all configured sensors and text sensors to the inverter instance
        for field_name in inverter_config:
            field_value = inverter_config.get(field_name)
            try:
                sensor_type = str(field_value.get('id').type);
                if sensor_type == 'sensor::Sensor':
                    await make_sensor(field_name)
                elif sensor_type == 'text_sensor::TextSensor':
                    await make_text_sensor(field_name)
            except Exception as e:
                pass

        # add inverter to component
        cg.add(component.add_inverter(inverter))
