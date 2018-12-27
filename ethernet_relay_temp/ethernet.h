#include <UIPEthernet.h>
#include "PubSubClient.h"

EthernetClient ethClient;
PubSubClient mqttClient;

uint8_t mac[6];
uint8_t mqtt_fail_counter = 0;

void mqttMsg(char* topic, uint8_t* payload, unsigned int length) {
#if defined(ENABLE_DEBUG)
    debugPort.print("Message arrived [");
    debugPort.print(topic);
    debugPort.println("] ");
#endif

    if (strcmp(topic, MQTT_RELAY_TOPIC_SET) == 0) {
        if (payload[0] == MQTT_RELAY_ON[0]) {
#if defined(ENABLE_DEBUG)
            debugPort.println("Turning relay ON");
#endif
            digitalWrite(RELAY_PIN, HIGH);

            sendData(MQTT_RELAY_TOPIC_STATE, MQTT_RELAY_ON, true);

        } else if (payload[0] == MQTT_RELAY_OFF[0]) {
#if defined(ENABLE_DEBUG)
            debugPort.println("Turning relay OFF");
#endif
            digitalWrite(RELAY_PIN, LOW);

            sendData(MQTT_RELAY_TOPIC_STATE, MQTT_RELAY_OFF, true);
        }

    } else if (strcmp(topic, MQTT_TOPIC_RESTART) == 0) {
        HAL_NVIC_SystemReset();
    }
}

void mqttConnect() {
    IWatchdog.reload();

#if defined(ENABLE_DEBUG)
    debugPort.print("Connecting to MQTT (");
    debugPort.print(MQTT_HOST);
    debugPort.print(")...");
#endif

    if (mqttClient.connect(MQTT_ID, MQTT_USER, MQTT_PASS, MQTT_TOPIC_WILL, 0, true, MQTT_STATUS_OFF)) {
#if defined(ENABLE_DEBUG)
        debugPort.println("connected");
#endif
        IWatchdog.reload();
        mqtt_fail_counter = 0;

        sendData(MQTT_TOPIC_WILL, MQTT_STATUS_ON, true);
        mqttClient.subscribe(MQTT_RELAY_TOPIC_SET);

    } else {
#if defined(ENABLE_DEBUG)
        debugPort.println("failed");
        delay(100);
#endif
        mqtt_fail_counter++;

        if (mqtt_fail_counter > MQTT_MAX_FAILED_CONNECTIONS) {
            HAL_NVIC_SystemReset();
        }
    }

    IWatchdog.reload();
}

String DisplayAddress(IPAddress address) {
    return String(address[0]) + "." +
           String(address[1]) + "." +
           String(address[2]) + "." +
           String(address[3]);
}

void ethSetup() {
    pinMode(ETH_RST_PIN, OUTPUT);     // this lets you pull the pin low.
    digitalWrite(ETH_RST_PIN, LOW);   // this resets the ENC28J60 hardware
    delay(100);                       // this makes sure the ENC28j60 resets OK.
    digitalWrite(ETH_RST_PIN, HIGH);  // this makes for a fast rise pulse;
    pinMode(ETH_RST_PIN, INPUT);

    // build MAC
    uint32_t word0 = *(uint32_t*)(0x1FFFF7E8U);

    mac[0] = 0x00;  // STM default
    mac[1] = 0x80;  // STM default
    mac[2] = 0xe1;  // STM default
    mac[3] = (word0 & 0x00ff0000) >> 16;
    mac[4] = (word0 & 0x0000ff00) >> 8;
    mac[5] = (word0 & 0x000000ff);

    if (Ethernet.begin(mac) == 0) {
#if defined(ENABLE_DEBUG)
        debugPort.println("Ethernet configuration using DHCP failed");
#endif
        digitalWrite(INFO_LED, LOW);

        HAL_NVIC_SystemReset();

    } else {
#if defined(ENABLE_DEBUG)
        debugPort.print("DHCP ok, address: ");
        debugPort.println(DisplayAddress(Ethernet.localIP()));
#endif
    }
}

bool sendData(const char * topic, const char * data, bool retain) {
    if (!mqttClient.connected()) {
        return false;
    }

    return mqttClient.publish(topic, data, retain);
}
