/*
  Mqtt publisher over WiFi
  
  - The ArduinoMqttClient library is used to connect to the Mqtt Broker.
*/
#include <ArduinoMqttClient.h>
#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_AVR_UNO_WIFI_REV2)
  #include <WiFiNINA.h>
#elif defined(ARDUINO_SAMD_MKR1000)
  #include <WiFi101.h>
#elif defined(ARDUINO_ARCH_ESP8266)
  #include <ESP8266WiFi.h>
#elif defined(ARDUINO_PORTENTA_H7_M7) || defined(ARDUINO_NICLA_VISION) || defined(ARDUINO_ARCH_ESP32)
  #include <WiFi.h>
#endif

#include "arduino_secrets.h"                      // not required if using the online editor
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
const char ssid[] = SECRET_SSID;                  // your network SSID (name)
const char pass[] = SECRET_PASS;                  // your network password

// For static IP set the define to true and fill the wanted IP in connectToWiFi()
// For dynamic IP set the define to false
#define USE_STATIC_IP                 false

// Serial Debug
// - if USE_DPRINT is set to true, DPRINT, DPRINTLN, ... do output to Serial Monitor.
// - if USE_DPRINT is set to false, DPRINT, DPRINTLN, ... are optimized away.
#define USE_DPRINT                    true
#define DPRINT_SERIAL_SPEED           9600
#define DPRINT(...)                   do { if (USE_DPRINT) Serial.print(__VA_ARGS__); } while (false)
#define DPRINTLN(...)                 do { if (USE_DPRINT) Serial.println(__VA_ARGS__); } while (false)
#define DWRITE(...)                   do { if (USE_DPRINT) Serial.write(__VA_ARGS__); } while (false)
#define DPRINTWIFISTATUS(...)         do { if (USE_DPRINT) printWiFiStatus(__VA_ARGS__); } while (false)
#define DPRINTCLIENTSTATUS(...)       do { if (USE_DPRINT) printClientStatus(__VA_ARGS__); } while (false)
// Note: do-while(false) guards against if-else constructs without curly braces.

// Timeouts in ms
const unsigned long wifiStatusPollMs = 100;       // on first connection setup, poll the WiFi status with this rate
const unsigned long connectingRetryMs = 15000;    // do not set under 15 sec for the following two reasons:
                                                  // - reconnects would end-up to be too frequent (for both WiFi and Broker)
                                                  // - mqtt3.thingspeak.com requires at least 15s between published messages
unsigned long lastPollMillis;                     // millis() of the last poll

// WiFi and Mqtt clients
// To connect with SSL/TLS:
// 1. Change WiFiClient to WiFiSSLClient
// 2. Change port value from 1883 to 8883
// 3. Flash the SSL/TLS root certificate for the used broker
WiFiClient client;
MqttClient mqttClient(client);
const char broker[] = "mqtt3.thingspeak.com";     // "test.mosquitto.org" or "mqtt3.thingspeak.com"
int        port     = 1883;
const char topic[]  = SECRET_MQTT_TOPIC;

// Do not call this function directly, only through DPRINTWIFISTATUS
// so that the compiler can optimize it away when USE_DPRINT is set to false
static void printWiFiStatus(uint8_t wifiStatus)
{
  switch (wifiStatus)
  {
    case WL_IDLE_STATUS:          Serial.print(F("WL_IDLE_STATUS")); break;
    case WL_NO_SSID_AVAIL:        Serial.print(F("WL_NO_SSID_AVAIL")); break;
    case WL_SCAN_COMPLETED:       Serial.print(F("WL_SCAN_COMPLETED")); break;
    case WL_CONNECTED:            Serial.print(F("WL_CONNECTED")); break;
    case WL_CONNECT_FAILED:       Serial.print(F("WL_CONNECT_FAILED")); break;
    case WL_CONNECTION_LOST:      Serial.print(F("WL_CONNECTION_LOST")); break;
    case WL_DISCONNECTED:         Serial.print(F("WL_DISCONNECTED")); break;
    case WL_NO_SHIELD:            Serial.print(F("WL_NO_SHIELD")); break;
    default:                      Serial.print(wifiStatus); break;
  }
}

// Do not call this function directly, only through DPRINTCLIENTSTATUS
// so that the compiler can optimize it away when USE_DPRINT is set to false
static void printClientStatus(uint8_t bConnected)
{
  if (bConnected)
    Serial.print(F("CONNECTED"));
  else
    Serial.print(F("NOT CONNECTED"));
}

static void connectToWiFi()
{
   // Static IP
   // Note: set them with each re-connect as sometimes they get lost and are all 0.0.0.0
#if USE_STATIC_IP == true
  IPAddress ip(192, 168, 1, 28);              // or: byte ip[] = {192, 168, 1, 28};
  IPAddress dns(192, 168, 1, 1);              // DNS server, optional, it's not clear what's the default...
  IPAddress gateway(192, 168, 1, 1);          // network gateway, optional, defaults to the device IP address with the last byte set to 1
  IPAddress subnet(255, 255, 255, 0);         // subnet mask of the network, optional, defaults to 255.255.255.0
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
  WiFi.config(ip, gateway, subnet, dns);
#elif defined(ARDUINO_PORTENTA_H7_M7) || defined(ARDUINO_NICLA_VISION)
  WiFi.config(ip, subnet, gateway);
#else
  WiFi.config(ip, dns, gateway, subnet);
#endif
#endif

  // Begin
  DPRINT(F("Connecting to SSID     : "));
  DPRINTLN(ssid);
  WiFi.begin(ssid, pass);
}

static bool connectToMqtt()
{
  DPRINT(F("Connecting to Broker   : "));
  DPRINTLN(broker);
  if (mqttClient.connect(broker, port))
    return true;
  else
  {
    DPRINT(F("MQTT connection error  : "));
    DPRINTLN(mqttClient.connectError());
    return false;
  }
}

void setup()
{
  // Serial Debug
#if USE_DPRINT == true
  Serial.begin(DPRINT_SERIAL_SPEED);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!
#endif

  // MQTT client ID and credentials
#if defined(SECRET_MQTT_CLIENT_ID)
  mqttClient.setId(SECRET_MQTT_CLIENT_ID);
#endif
#if defined(SECRET_MQTT_USERNAME) && defined(SECRET_MQTT_PASSWORD)
  mqttClient.setUsernamePassword(SECRET_MQTT_USERNAME, SECRET_MQTT_PASSWORD);
#endif

  // Connect
  while (true)
  {
    // Connection setup
    lastPollMillis = millis();
    connectToWiFi(); // some platforms have a blocking WiFi.begin(), others a non-blocking

    // Poll till connected
    uint8_t wifiStatus;
    while ((wifiStatus = WiFi.status()) != WL_CONNECTED &&
          (millis() - lastPollMillis) < connectingRetryMs)
    {
      delay(wifiStatusPollMs);
    }

    // Connected?
    if (wifiStatus == WL_CONNECTED)
    {
      DPRINT(F("Arduino's IP address   : "));
      DPRINTLN(WiFi.localIP());
      DPRINT(F("Gateway's IP address   : "));
      DPRINTLN(WiFi.gatewayIP());
      DPRINT(F("Network's subnet mask  : "));
      DPRINTLN(WiFi.subnetMask());
      break;
    }
    else
    {
      DPRINT(F("Connection error status: "));
      DPRINTWIFISTATUS(wifiStatus); DPRINTLN();
    }
  }
  connectToMqtt();

  // Init poll var
  lastPollMillis = millis();
}

void loop()
{
  unsigned long currentMillis = millis();
  if (currentMillis - lastPollMillis > connectingRetryMs)
  {
    lastPollMillis = currentMillis;

    DPRINTLN(F("------------------------------------------"));
    
    // WiFi status poll and reconnect
    uint8_t wifiStatus = WiFi.status();
    DPRINT(F("WiFi status            : "));
    DPRINTWIFISTATUS(wifiStatus); DPRINTLN();
    DPRINT(F("Signal strength        : "));
    DPRINT(WiFi.RSSI()); DPRINTLN(F(" dBm"));
    if (wifiStatus != WL_CONNECTED)
      connectToWiFi();

    // Mqtt status poll and reconnect
    bool bDoPublish;
    if (WiFi.status() == WL_CONNECTED)
    {
      uint8_t bConnected = mqttClient.connected();
      DPRINT(F("Client status          : "));
      DPRINTCLIENTSTATUS(bConnected); DPRINTLN();
      if (!bConnected)
        bDoPublish = connectToMqtt();
      else
        bDoPublish = true;
    }
    else
      bDoPublish = false;

    // Publish
    if (bDoPublish)
    {
      int readValue1 = analogRead(A0);
      int readValue2 = analogRead(A1);
      int readValue3 = analogRead(A2);
      DPRINT(F("Publish message        : ")); DPRINT(topic); DPRINT(F(" => "));
      DPRINT(F("field1=")); DPRINT(readValue1);
      DPRINT(F("&field2=")); DPRINT(readValue2);
      DPRINT(F("&field3=")); DPRINTLN(readValue3);
      mqttClient.beginMessage(topic);
      mqttClient.print(F("field1=")); mqttClient.print(readValue1);
      mqttClient.print(F("&field2=")); mqttClient.print(readValue2);
      mqttClient.print(F("&field3=")); mqttClient.print(readValue3);
      mqttClient.endMessage();
    }
  }

  // Send MQTT keep alive which avoids being disconnected by the broker
  mqttClient.poll();
}
