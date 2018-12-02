# STM32F103C8 (BluePill) based temperature node with relay
[![Build Status](https://travis-ci.org/pilotak/ethernet-relay-temp.svg?branch=master)](https://travis-ci.org/pilotak/ethernet-relay-temp)

## Features
- variable power supply: 5-15V (depends on relay coil)
- 1-wire bus for DS18B20 temperature sensors
- 1x thermistor input
- 1x SPST relay with ARC suppression
- controlled over MQTT

## MQTT topics
- `nameOfBoard/dtemp/*` = dallas temperature *(aterisk is temp address)*
- `nameOfBoard/atemp/*` = thermistor temperature *(aterisk is analog temp index)*
- `nameOfBoard/status` = online status
- `nameOfBoard/err` = dallas error, tells you index of the bad sensor
- `nameOfBoard/count` = number dallas sensors found
- `nameOfBoard/relay/0` = state of relay
- `nameOfBoard/relay/0/set` = command for relay, can be either `0` or `1`

## BOM
| Qty | Component | Value | Package |
|:---:| --------- | ----- | ------- |
| 1 | Ceramic capacitor | 10uF | C0805 |
| 1 | Electrolytic capacitor | 100uF | E2-5 |
| 1 | Polypropylene capacitor | R463I24705001K | C150-084X183 |
| 1 | Diode | S1B |DO214AC (SMA) |
| 1 | Shottky diode | SS16 | DO214AC (SMA) |
| 1 | Barrel jack | 2.1mm | |
| 1 | Relay | RM85-2021-35-1005 | |
| 1 | NPN transistor | BC817 | SOT23-BEC |
| 1 | Resistor | 1K | R0805 |
| 1 | Resistor | 4K7 | R0805 |
| 1 | Resistor | 10K | R0805 |
| 1 | Resistor | 160R | R2010 |
| 1 | Ethernet module | ENC28J60 | |
| 1 | LDO regulator | LM1117-3.3 | SOT223 |
| 1 | Processor module | STM32F103C8T6 | |
| 2 | Terminal | Wago 236-740-5,08 | |
| 1 | Terminal | KF350-3.50-2P | |
| 3 | Terminal | KF350-3.50-3P | |
| 1 | Case | KRADEX Z76 | |

