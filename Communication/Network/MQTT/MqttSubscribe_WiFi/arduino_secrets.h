// WiFi
#define SECRET_SSID "MY_SSID"
#define SECRET_PASS "MY_PASSWORD"

// *** Uncomment Option A. OR Option B. ***

// A. To use the test.mosquitto.org broker only define SECRET_MQTT_TOPIC 
//    which must be the same for both the publisher and the subscriber
//#define SECRET_MQTT_TOPIC     "myunique/topic/MY_CH_NUM"

// B. To use the mqtt3.thingspeak.com broker it's necessary to create a free 
//    account at thingspeak.com. Then setup a channel with an associated MQTT
//    device. Here provide the client ID + credentials + channel number.
//    Attention: when running one device as publisher and another as subscriber
//    you have to use different client IDs!
#define SECRET_MQTT_CLIENT_ID "MY_MQTT_CLIENT_ID2"
#define SECRET_MQTT_USERNAME  "MY_MQTT_USERNAME2"
#define SECRET_MQTT_PASSWORD  "MY_MQTT_PASSWORD2"
#define SECRET_MQTT_TOPIC     "channels/MY_CH_NUM/subscribe" // set the channel number here
