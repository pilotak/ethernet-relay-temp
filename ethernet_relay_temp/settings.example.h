#define SENSORS_SEND_INTERVAL 32000  // ms
#define SENSORS_READ_INTERVAL 1000  // ms
#define MQTT_STATUS_SEND_INTERVAL 30000  // ms
#define WDT_INTERVAL 2000 // ms, feed the dog interval; for setup see watchdog.h

#define MQTT_ID       "someName"
#define MQTT_HOST     "192.168.0.1"
#define MQTT_USER     "admin"
#define MQTT_PASS     ""
#define MQTT_PORT     1883

#define MQTT_BASE_TOPIC MQTT_ID
#define MQTT_TOPIC_DALLAS_TEMP MQTT_BASE_TOPIC"/dtemp"  // will result ie. someName/dtemp/* where aterisk is temp address
#define MQTT_TOPIC_ANALOG_TEMP MQTT_BASE_TOPIC"/atemp"  // will result ie. someName/atemp/* where aterisk is analog temp index
#define MQTT_TOPIC_WILL MQTT_BASE_TOPIC"/status"        // will result ie. someName/status
#define MQTT_TOPIC_ERROR MQTT_BASE_TOPIC"/err"          // will result ie. someName/err
#define MQTT_TOPIC_COUNT MQTT_BASE_TOPIC"/count"        // will result ie. someName/count
#define MQTT_STATUS_ON "1"
#define MQTT_STATUS_OFF "0"
#define MQTT_RELAY_TOPIC_STATE MQTT_BASE_TOPIC"/relay/0"   // will result ie. someName/relay/0
#define MQTT_RELAY_TOPIC_SET MQTT_RELAY_TOPIC_STATE"/set"  // will result ie. someName/relay/0/set
#define MQTT_RELAY_ON "1"
#define MQTT_RELAY_OFF "0"
#define MQTT_TOPIC_RESTART MQTT_BASE_TOPIC"/restart"  // will result ie. someName/restart
#define MQTT_MAX_FAILED_CONNECTIONS 10

#define INFO_LED LED_BUILTIN

#if defined(ENABLE_DEBUG)
    #define debugPort Serial2
#endif

#define RELAY_PIN PA0
#define ONE_WIRE_PIN PB9
#define ETH_RST_PIN PB0
#define ANALOG_TEMPS 1
const int a_temp_pin[ANALOG_TEMPS] = {PB1};
#define RELAY_PIN PA0
#define MAX_DS18B20 3
#define TEMPERATURE_PRECISION 11

// resistance at 25 degrees C
#define THERMISTOR_NOMINAL 10000
// temp. for nominal resistance (almost always 25 C)
#define TEMPERATURE_NOMINAL 25
// The beta coefficient of the thermistor (usually 3000-4000)
#define BCOEFFICIENT 3950
// the value of the 'other' resistor
#define A_TEMP_SERIES_RESISTOR 10000


#define MQTT_RELAY_TOPIC_RELAY_STATE MQTT_BASE_TOPIC "/relay"  // will result ie. someName/relay/* where aterisk is relay number
#define MQTT_RELAY_TOPIC_SET_APPENDIX "set"  // will result ie. someName/relay/*/set
#define MQTT_STATE_ON "1"
#define MQTT_STATE_OFF "0"