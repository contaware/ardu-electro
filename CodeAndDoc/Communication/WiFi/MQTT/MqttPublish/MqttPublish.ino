/*
  Mqtt publisher over WiFi
  
  - The ArduinoMqttClient library is used to connect to the Mqtt Broker.
*/
#include <ArduinoMqttClient.h>
#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_AVR_UNO_WIFI_REV2) || defined(ARDUINO_NANO_RP2040_CONNECT)
  #include <WiFiNINA.h>
#elif defined(ARDUINO_SAMD_MKR1000)
  #include <WiFi101.h>
#elif defined(ARDUINO_ARCH_ESP8266)
  #include <ESP8266WiFi.h>
  #include <WiFiClientSecure.h>
#elif defined(ARDUINO_ARCH_ESP32)
  #include <WiFi.h>
  #include <WiFiClientSecure.h>
#elif defined(ARDUINO_PORTENTA_H7_M7) || defined(ARDUINO_NICLA_VISION) || defined(ARDUINO_GIGA)
  #include <WiFi.h>
#elif defined(ARDUINO_UNOR4_WIFI)
  #include <WiFiS3.h>
  #include <WiFiSSLClient.h>
#elif defined(ARDUINO_PORTENTA_C33)
  #include <WiFiC3.h>
  #include <WiFiSSLClient.h>
#endif
#include "trusted_root_certs.h"
#include "arduino_secrets.h"                      // not required if using the online editor
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
const char ssid[] = SECRET_SSID;                  // your network SSID (name)
const char pass[] = SECRET_PASS;                  // your network password

// For a normal connection set the define to false
// For a secure connection set the define to true
#define USE_SECURE_CONNECTION         false

// For static IP set the define to true and fill the wanted IP in connectToWiFi()
// For dynamic IP set the define to false
#define USE_STATIC_IP                 false

// Set your time zone
// https://remotemonitoringsystems.ca/time-zone-abbreviations.php
// https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
// https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
const char* MY_TZ_INFO = "CET-1CEST,M3.5.0,M10.5.0/3";

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
const unsigned long connectingRetryMs = 20000;    // do not set under 15 sec for the following two reasons:
                                                  // - reconnects would end-up to be too frequent (for both WiFi and Broker)
                                                  // - mqtt3.thingspeak.com requires at least 15s between published messages
unsigned long lastPollMillis;                     // millis() of the last poll

// WiFi and Mqtt clients
// - For ESP8266/ESP32 the certificates are in trusted_root_certs.h.
// - For the other Arduinos, flash the SSL/TLS root certificates for
//   the used brokers in the Arduino IDE; note that when flashing 
//   new certificates all the previous ones are erased. Supplying 
//   test.mosquitto.org:8883 in the Arduino IDE is not working,
//   the solution is:
//   1. Download the mosquitto certificate from:
//      https://test.mosquitto.org/ssl/mosquitto.org.crt 
//   2. Rename mosquitto.org.crt to mosquitto.org.pem.
//   3. Install arduino-fwuploader and run the following command:
//      arduino-fwuploader certificates flash --url arduino.cc:443,mqtt3.thingspeak.com:8883 --file "C:\your_path\mosquitto.org.pem" -b arduino:samd:mkrwifi1010 -a COM13
//      (get the board names running arduino-fwuploader firmware list)
#if USE_SECURE_CONNECTION == true
  #if defined(ARDUINO_ARCH_ESP8266)
    WiFiClientSecure client;
    X509List trustedRootCerts;
  #elif defined(ARDUINO_ARCH_ESP32)
    WiFiClientSecure client;
  #else
    WiFiSSLClient client;
  #endif
  const int port = 8883;
#else
  WiFiClient client;
  const int port = 1883;
#endif
MqttClient mqttClient(client);
const char broker[] = "mqtt3.thingspeak.com";     // "test.mosquitto.org" or "mqtt3.thingspeak.com"
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
#elif defined(ARDUINO_PORTENTA_H7_M7) || defined(ARDUINO_NICLA_VISION) || defined(ARDUINO_GIGA)
  WiFi.config(ip, subnet, gateway);
#else
  WiFi.config(ip, dns, gateway, subnet);
#endif
#endif

  // Begin
  DPRINT(F("Connecting to SSID     : "));
  DPRINTLN(ssid);
#if USE_DPRINT == true
  unsigned long startMillis = millis();
#endif
  WiFi.begin(ssid, pass);
#if USE_DPRINT == true
  unsigned long endMillis = millis();
  DPRINT(F("                         [call time="));
  DPRINT(endMillis - startMillis); DPRINTLN(F(" ms]"));
#endif
}

static bool connectToMqtt()
{
  // Connect
  DPRINT(F("Connecting to Broker   : "));
  DPRINT(broker); DPRINT(F(":")); DPRINTLN(port);
#if USE_DPRINT == true
  unsigned long startMillis = millis();
#endif
  int ret = mqttClient.connect(broker, port);
#if USE_DPRINT == true
  unsigned long endMillis = millis();
  DPRINT(F("                         [call time="));
  DPRINT(endMillis - startMillis); DPRINTLN(F(" ms]"));
#endif

  // Return
  if (ret)
    return true;
  else
  {
    DPRINT(F("MQTT connection error  : "));
    DPRINTLN(mqttClient.connectError());
    return false;
  }
}

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
bool initNTPTime(unsigned long blockForMs = 30000UL)
{
  DPRINT(F("NTP init current time  : "));
  #define SECS_YR_2000 ((time_t)(946684800UL)) // the time at the start of y2k
  configTzTime(MY_TZ_INFO, "pool.ntp.org", "time.nist.gov", "time.windows.com");
  time_t nowTime;
  unsigned long startMillis = millis();
  do
  {
    time(&nowTime); // time() calls the NTP server every hour
    delay(100);
  } while ((millis() - startMillis) <= blockForMs && nowTime < SECS_YR_2000);
  if (nowTime < SECS_YR_2000) // the NTP call was not successful
  {
    DPRINTLN(F("failed"));
    return false;
  }
#if USE_DPRINT == true
  char sTime[26]; // ctime_r() needs a buffer of 24 chars + \n + \0
  DPRINT(ctime_r(&nowTime, sTime));
#endif
  return true;
}
#endif

void setup()
{
  // Serial Debug
#if USE_DPRINT == true
  Serial.begin(DPRINT_SERIAL_SPEED);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!
  delay(5000);      // for ESP32 and some other MCUs a delay() is needed, otherwise
                    // the messages generated in setup() can't be seen!
#endif

  // Setup Trusted Root Certificate(s) for ESP8266/ESP32
#if USE_SECURE_CONNECTION == true
#if defined(ARDUINO_ARCH_ESP8266)
  trustedRootCerts.append(certMosquittoOrg);
  trustedRootCerts.append(certDigiCertGlobalRootG2);
  client.setTrustAnchors(&trustedRootCerts);
#elif defined(ARDUINO_ARCH_ESP32)
  client.setCACert(certDigiCertGlobalRootG2); // certMosquittoOrg or certDigiCertGlobalRootG2
#endif
#endif

  // MQTT client ID and credentials
#if defined(SECRET_MQTT_CLIENT_ID)
  mqttClient.setId(SECRET_MQTT_CLIENT_ID);
#endif
#if defined(SECRET_MQTT_USERNAME) && defined(SECRET_MQTT_PASSWORD)
  mqttClient.setUsernamePassword(SECRET_MQTT_USERNAME, SECRET_MQTT_PASSWORD);
#endif

  // Connect to WiFi
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
      DPRINTLN((IPAddress)WiFi.localIP());    // cast because some libs return uint32_t instead of IPAddress
      DPRINT(F("Gateway's IP address   : "));
      DPRINTLN((IPAddress)WiFi.gatewayIP());  // cast because some libs return uint32_t instead of IPAddress
      DPRINT(F("Network's subnet mask  : "));
      DPRINTLN((IPAddress)WiFi.subnetMask()); // cast because some libs return uint32_t instead of IPAddress
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_UNOR4_WIFI)
      DPRINT(F("DNS's IP address       : "));
      DPRINTLN((IPAddress)WiFi.dnsIP());      // cast because some libs return uint32_t instead of IPAddress
#endif
      break;
    }
    else
    {
      DPRINT(F("Connection error status: "));
      DPRINTWIFISTATUS(wifiStatus); DPRINTLN();
    }
  }

  // Initialize the time through the NTP protocol
  // Note: ESP8266 needs a correct time to verify the certificates,
  //       ESP32 does not need it, but it does not harm to have the time.
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
  initNTPTime();
#endif

  // Connect to MQTT
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
      connectToWiFi(); // some platforms have a blocking WiFi.begin(), others a non-blocking

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
      long exampleValue1 = random(0, 100); // 0..99
      long exampleValue2 = random(0, 100); // 0..99
      DPRINT(F("Publish message        : ")); DPRINT(topic); DPRINT(F(" => "));
      DPRINT(F("field1=")); DPRINT(exampleValue1);
      DPRINT(F("&field2=")); DPRINTLN(exampleValue2);
      mqttClient.beginMessage(topic);
      mqttClient.print(F("field1=")); mqttClient.print(exampleValue1);
      mqttClient.print(F("&field2=")); mqttClient.print(exampleValue2);
      mqttClient.endMessage();
    }
  }

  // Send MQTT keep alive which avoids being disconnected by the broker
  mqttClient.poll();
}
