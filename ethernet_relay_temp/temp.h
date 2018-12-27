#include <OneWire.h>
#include <DallasTemperature.h>
#include "Hysteresis.h"
#include "MovingAverage.h"

OneWire oneWire(ONE_WIRE_PIN);

DallasTemperature sensors(&oneWire);
DeviceAddress dallasAddresses[MAX_DS18B20];
Hysteresis <int32_t> dHysteresis[MAX_DS18B20];
MovingAverage <int32_t, 32> dAverage[MAX_DS18B20];
Hysteresis <int32_t> aHysteresis[ANALOG_TEMPS];
MovingAverage <int32_t, 32> aAverage[ANALOG_TEMPS];

char address_string[MAX_DS18B20][17];
const char * hex = "0123456789ABCDEF";

uint8_t ds12b20_count = 0;

void tempSetup() {
    for (uint8_t i = 0; i < ANALOG_TEMPS; i++) {
        pinMode(a_temp_pin[i], INPUT_ANALOG);
        aHysteresis[i].set(10);
        aHysteresis[i].prev(LONG_MIN);
    }

    sensors.begin();
    ds12b20_count = sensors.getDeviceCount();

#if defined(ENABLE_DEBUG)
    debugPort.print("Found ");
    debugPort.print(ds12b20_count, DEC);
    debugPort.println(" devices");
#endif

    ds12b20_count = (ds12b20_count > MAX_DS18B20 ? MAX_DS18B20 : ds12b20_count);  // prevent overflow

    char data[2];
    data[0] = ds12b20_count + '0';
    data[1] = 0;
    sendData(MQTT_TOPIC_COUNT, data, true);

    if (ds12b20_count != 0) {
        for (uint8_t i = 0; i < ds12b20_count; i++) {
            dHysteresis[i].set(10);
            dHysteresis[i].prev(LONG_MIN);

            if (sensors.getAddress(dallasAddresses[i], i)) {
                sensors.setResolution(dallasAddresses[i], TEMPERATURE_PRECISION);

                for (uint8_t j = 0; j < 8; ++j) {
                    address_string[i][(j << 1)] = hex[((uint8_t)dallasAddresses[i][j] >> 4) & 0xF];
                    address_string[i][(j << 1) + 1] = hex[dallasAddresses[i][j] & 0xF];
                }

                address_string[i][16] = 0;  // terminate string

            } else {
                char data[3];
                data[0] = '-';
                data[1] = i + '0';
                data[2] = 0;
                sendData(MQTT_TOPIC_ERROR, data, true);

#if defined(ENABLE_DEBUG)
                debugPort.print("Sending message: Dallas ");
                debugPort.print(i);
                debugPort.println(" error");
#endif
            }

            iwdg_feed();
        }
    }
}

void readTemp() {
    float adc;
    float steinhart;
    uint8_t i = 0;
    int32_t temp;

    if (ds12b20_count > 0) {
        sensors.requestTemperatures();
    }

    for (i = 0; i < ds12b20_count; i++) {
        float tempC = sensors.getTempC(dallasAddresses[i]);

#if defined(ENABLE_DEBUG)
        debugPort.print("Dallas temp[");
        debugPort.print(i);
        debugPort.print("]: ");
        debugPort.println(tempC, 3);
#endif

        if (tempC != -127.0) {
            dHysteresis[i].add(dAverage[i].add(static_cast<int32_t>(tempC * 100)));

        } else {
            dHysteresis[i].prev(LONG_MIN);
            dAverage[i].reset();
        }

        iwdg_feed();
    }

    for (i = 0; i < ANALOG_TEMPS; i++) {
        adc = analogRead(a_temp_pin[i]);

        if (adc < 4080) {
            adc = (4095 / adc) - 1;
            adc = A_TEMP_SERIES_RESISTOR / adc;

            steinhart = adc / THERMISTOR_NOMINAL;               // (R/Ro)
            steinhart = log(steinhart);                         // ln(R/Ro)
            steinhart /= BCOEFFICIENT;                          // 1/B * ln(R/Ro)
            steinhart += 1.0 / (TEMPERATURE_NOMINAL + 273.15);  // + (1/To)
            steinhart = 1.0 / steinhart;                        // Invert
            steinhart -= 273.15;                                // convert to C


#if defined(ENABLE_DEBUG)
            debugPort.print("Analog temp[");
            debugPort.print(i);
            debugPort.print("]: ");
            debugPort.println(steinhart, 3);
#endif

            aHysteresis[i].add(aAverage[i].add(static_cast<int32_t>(steinhart * 100.0)));

        } else {
            aHysteresis[i].prev(LONG_MIN);
            aAverage[i].reset();

#if defined(ENABLE_DEBUG)
            debugPort.print("Invalid ADC reading[");
            debugPort.print(i);
            debugPort.println("]");
#endif
        }
    }

#if defined(ENABLE_DEBUG)
    debugPort.println();
#endif
}

void sendTemp() {
    char data[20];
    char topic[50];
    uint8_t i = 0;
    int32_t temp;
    float temp_float;

#if defined(ENABLE_DEBUG)
    debugPort.println("Sending temperatures\n");
#endif

    digitalWrite(INFO_LED, LOW);

    for (i = 0; i < ds12b20_count; i++) {
        temp_float = NAN;
        temp = dHysteresis[i].get();

        if (temp > LONG_MIN) {
            temp_float = static_cast<float>(temp) / 100.0;
        }

        snprintf(data, sizeof(data), "%.1f", temp_float);
        snprintf(topic, sizeof(topic), "%s/%s", MQTT_TOPIC_DALLAS_TEMP, address_string[i]);
        sendData(topic, data, true);

        iwdg_feed();
    }

    for (i = 0; i < ANALOG_TEMPS; i++) {
        temp_float = NAN;
        temp = aHysteresis[i].get();

        if (temp > LONG_MIN) {
            temp_float = static_cast<float>(temp) / 100.0;
        }

        snprintf(data, sizeof(data), "%.1f", temp_float);
        snprintf(topic, sizeof(topic), "%s/%u", MQTT_TOPIC_ANALOG_TEMP, i);
        sendData(topic, data, true);

        iwdg_feed();
    }

    digitalWrite(INFO_LED, HIGH);
}
