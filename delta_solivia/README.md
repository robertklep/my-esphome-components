# Delta Solivia RS485 component for ESPHome

This component for ESPHome can be used to directly read measurement/statistics data from the RS485 port of _some_ Delta Solivia inverters. It does not require having a Solivia Gateway device, but will function just fine if you have one.

## SYNOPSIS

The document "Public Solar Inverter Communication Protocol (Version 1.2)", from Delta Energy Systems (Germany) GmbH, documents the protocol that Delta Solivia inverters use over their RS485 port.

There are dozens of Solivia models, and about 10 different protocol variants.

This component only supports one variant, supported by the following inverters:
* SOLIVIA 2.5 EU G3
* SOLIVIA 3.0 EU G3
* SOLIVIA 3.3 EU G3
* SOLIVIA 3.6 EU G3
* SOLIVIA 2.5 NA G4
* SOLIVIA 3.0 NA G4
* SOLIVIA 3.3 NA G4
* SOLIVIA 3.6 NA G4
* SOLIVIA 4.4 NA G4
* SOLIVIA 5.0 NA G4
* SOLIVIA 2.5 AP G3
* SOLIVIA 3.0 AP G3
* SOLIVIA 3.3 AP G3
* SOLIVIA 3.6 AP G3

The component does not perform any checks to see if it's talking to a supported inverter, but just assumes it does, so using it for a different type of inverter will probably cause the component to either crash or to provide incorrect data. Nothing bad will happen with your inverter if you do try this.

## HARDWARE

### MCU

While not tested, the component should work on most ESP32 boards, and probably also ESP8266 boards. I'm using an ESP32-C3 "SuperMini" board.

### MAX485 RS485-to-TTL module

These modules are cheap and easy to get. They require explicit flow control (see below, "ESPHome setup") for the component to be able to write to the RS485 bus.

##### Pinout

| ESP32 | RS485 module | Purpose      |
|-------|--------------|--------------|
| GPIO4 | DI           | TX           |
| GPIO3 | RO           | RX           |
| GPIO2 | RE + DE      | Flow control |

### Automatic flow control

RS485 boards that have automatic flow control will take care of the correct signalling automatically.

## ESPHome setup

```
# Load as external component
external_components:
  - source: github://robertklep/my-esphome-components
    components: [ delta_solivia ]

# UART setup (change tx_pin/rx_pin if required, leave the rest as-is)
uart:
  tx_pin: GPIO4
  rx_pin: GPIO3
  id: solivia_uart
  rx_buffer_size: 512
  baud_rate: 19200
  parity: NONE
  stop_bits: 1

# This output is required for manual flow control, even if you don't
# write to the bus (it causes the flow control pin to be pulled down
# by default, which is required to be able to read from the RS485 module).
output:
  - platform: gpio
    pin: GPIO2
    id: flow_control

# component setup
delta_solivia:
  # The UART configured above
  uart_id: solivia_uart

  # Some inverters send measurement data once every few seconds,
  # especially if you have a gateway attached. This setting throttles
  # down the amount of updates sent to Home Assistant.
  #
  # Value in milliseconds.
  throttle: 10000

  # Here you can configure multiple inverters (at most 4, although that
  # maximum is arbitrary).
  #
  # Each inverter has a unique address, usually starting with "1" for the
  # first inverter, "2" for the second, etc.
  #
  # You can run the component with debug logging in ESPHome enabled,
  # which will log information on the packets it receives, including
  # the address from which the packet originated.
  #
  # For each inverter, a number of sensors is available. If you're not
  # interested in a specific sensor, just leave it out.
  inverters:
    - address: 1
      ac_power:
        name: 'Inverter#1 Current Power'
      total_energy:
        name: 'Inverter#1 Total Energy'
      today_energy:
        name: 'Inverter#1 Today Energy'
      dc_voltage:
        name: 'Inverter#1 DC Voltage'
      dc_current:
        name: 'Inverter#1 DC Current'
      ac_voltage:
        name: 'Inverter#1 AC Voltage'
      ac_current:
        name: 'Inverter#1 AC Current'
      ac_frequency:
        name: 'Inverter#1 AC Frequency'
      grid_voltage:
        name: 'Inverter#1 Grid Voltage'
      grid_frequency:
        name: 'Inverter#1 Grid Frequency'
      runtime_hours:
        name: 'Inverter#1 Runtime Hours'
      runtime_minutes:
        name: 'Inverter#1 Runtime Minutes'
      max_ac_power_today:
        name: 'Inverter#1 AC Power Today'
      max_solar_input_power:
        name: 'Inverter#1 Solar Input Power'
    - address: 2
      ac_power:
        name: 'Inverter#2 Current Power'
      total_energy:
        name: 'Inverter#2 Total Energy'
      today_energy:
        name: 'Inverter#2 Today Energy'
      dc_voltage:
        name: 'Inverter#2 DC Voltage'
      dc_current:
        name: 'Inverter#2 DC Current'
      ac_voltage:
        name: 'Inverter#2 AC Voltage'
      ac_current:
        name: 'Inverter#2 AC Current'
      ac_frequency:
        name: 'Inverter#2 AC Frequency'
      grid_voltage:
        name: 'Inverter#2 Grid Voltage'
      grid_frequency:
        name: 'Inverter#2 Grid Frequency'
      runtime_hours:
        name: 'Inverter#2 Runtime Hours'
      runtime_minutes:
        name: 'Inverter#2 Runtime Minutes'
      max_ac_power_today:
        name: 'Inverter#2 AC Power Today'
      max_solar_input_power:
        name: 'Inverter#2 Solar Input Power'
