/*
MIT License
Copyright (c) 2018 Pavel Slama
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <climits>
#include <Wire.h>
#include "settings.example.h"
#include "watchdog.h"

bool sendData(const char * topic, const char * data, bool retain = false);  // compiler workaround

#include "temp.h"
#include "ethernet.h"

uint32_t previousSendMillis = 0;
uint32_t previousWillMillis = 0;
uint32_t previousReadMillis = 0;
uint32_t previousWdtMillis = 0;

void setup() {
#if defined(ENABLE_DEBUG)
    debugPort.begin(115200);
#endif

    pinMode(INFO_LED, OUTPUT);
    digitalWrite(INFO_LED, HIGH);

    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW);

#if defined(ENABLE_DEBUG)
    debugPort.println("Starting");
#endif

    ethSetup();
    wdtSetup();

    // setup mqtt client
    mqttClient.setClient(ethClient);
    mqttClient.setServer(MQTT_HOST, MQTT_PORT);
    mqttClient.setCallback(mqttMsg);
    mqttConnect();

    tempSetup();

    previousSendMillis = previousWillMillis = millis();
}

void loop() {
    if (millis() - previousWdtMillis >= WDT_INTERVAL) {
        previousWdtMillis = millis();
        iwdg_feed();
    }

    if (millis() - previousReadMillis >= SENSORS_READ_INTERVAL) {
        previousReadMillis = millis();
        readTemp();
    }

    if (millis() - previousWillMillis >= MQTT_STATUS_SEND_INTERVAL) {
        previousWillMillis = millis();

        if (!sendData(MQTT_TOPIC_WILL, MQTT_STATE_ON, true)) {
            mqttConnect();
        }
    }

    if (millis() - previousSendMillis >= SENSORS_SEND_INTERVAL) {
        previousSendMillis = millis();
        sendTemp();
    }

    mqttClient.loop();
}
