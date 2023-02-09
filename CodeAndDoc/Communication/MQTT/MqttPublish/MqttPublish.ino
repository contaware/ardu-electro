/*
  Mqtt publisher over WiFi for Arduino MKR WiFi 1010, Arduino MKR VIDOR 4000,
  Arduino UNO WiFi Rev.2 and Nano 33 IoT
  
  - The ArduinoMqttClient library is used to connect to the Mqtt Broker.

  - This example uses no secure mqtt connections.
*/
#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>

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
#define DPRINTCLIENTSERVERSTATUS(...) do { if (USE_DPRINT) printClientServerStatus(__VA_ARGS__); } while (false)
// Note: do-while(false) guards against if-else constructs without curly braces.

// Timeouts in ms
const unsigned long connectionTimeoutMs = 50000;  // connection timeout for WiFi.begin(), default is 50 sec
const unsigned long connectingRetryMs = 10000;    // do not set under 10 sec, otherwise WiFi.begin() tries to login too often
unsigned long lastPollMillis;                     // millis() of the last poll

// WiFi and Mqtt clients
WiFiClient client;
MqttClient mqttClient(client);
const char broker[] = "test.mosquitto.org";
int        port     = 1883;
const char topic0[] = "my12743734562/unique_topic0";
const char topic1[] = "my12743734562/unique_topic1";
const char topic2[] = "my12743734562/unique_topic2";

// Use this function like DPRINT(freeMemory()), so that the compiler
// can optimize it away when USE_DPRINT is set to false
// freeMemory() code from https://github.com/mpflaga/Arduino-MemoryFree
#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__
static int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}

// Do not call this function directly, only through DPRINTWIFISTATUS
// so that the compiler can optimize it away when USE_DPRINT is set to false
static void printWiFiStatus(uint8_t wifiStatus)
{
  // See enum wl_status_t in wl_definitions.h
  switch (wifiStatus)
  {
    case WL_IDLE_STATUS:          Serial.print(F("WL_IDLE_STATUS")); break;
    case WL_NO_SSID_AVAIL:        Serial.print(F("WL_NO_SSID_AVAIL")); break;
    case WL_SCAN_COMPLETED:       Serial.print(F("WL_SCAN_COMPLETED")); break;
    case WL_CONNECTED:            Serial.print(F("WL_CONNECTED")); break;
    case WL_CONNECT_FAILED:       Serial.print(F("WL_CONNECT_FAILED")); break;
    case WL_CONNECTION_LOST:      Serial.print(F("WL_CONNECTION_LOST")); break;
    case WL_DISCONNECTED:         Serial.print(F("WL_DISCONNECTED")); break;
    case WL_AP_LISTENING:         Serial.print(F("WL_AP_LISTENING")); break;
    case WL_AP_CONNECTED:         Serial.print(F("WL_AP_CONNECTED")); break;
    case WL_AP_FAILED:            Serial.print(F("WL_AP_FAILED")); break;
    case WL_NO_MODULE:            Serial.print(F("WL_NO_MODULE or WL_NO_SHIELD")); break;
    default:                      Serial.print(wifiStatus); break;
  }
}

// Do not call this function directly, only through DPRINTCLIENTSERVERSTATUS
// so that the compiler can optimize it away when USE_DPRINT is set to false
static void printClientServerStatus(uint8_t clientServerStatus)
{
  // See enum wl_tcp_state in wifi_spi.h
  switch (clientServerStatus)
  {
    case CLOSED:                  Serial.print(F("CLOSED")); break;
    case LISTEN:                  Serial.print(F("LISTEN")); break;
    case SYN_SENT:                Serial.print(F("SYN_SENT")); break;
    case SYN_RCVD:                Serial.print(F("SYN_RCVD")); break;
    case ESTABLISHED:             Serial.print(F("ESTABLISHED")); break;
    case FIN_WAIT_1:              Serial.print(F("FIN_WAIT_1")); break;
    case FIN_WAIT_2:              Serial.print(F("FIN_WAIT_2")); break;
    case CLOSE_WAIT:              Serial.print(F("CLOSE_WAIT")); break;
    case CLOSING:                 Serial.print(F("CLOSING")); break;
    case LAST_ACK:                Serial.print(F("LAST_ACK")); break;
    case TIME_WAIT:               Serial.print(F("TIME_WAIT")); break;
    default:                      Serial.print(clientServerStatus); break;
  }
}

static bool connectToWiFi()
{
   // Static IP
   // Note: set them with each re-connect as sometimes they get lost and are all 0.0.0.0
#if USE_STATIC_IP == true
  IPAddress ip(192, 168, 1, 28);              // or: byte ip[] = {192, 168, 1, 28};
  IPAddress dns(192, 168, 1, 1);              // DNS server, optional, it's not clear what's the default...
  IPAddress gateway(192, 168, 1, 1);          // network gateway, optional, defaults to the device IP address with the last byte set to 1
  IPAddress subnet(255, 255, 255, 0);         // subnet mask of the network, optional, defaults to 255.255.255.0
  WiFi.config(ip, dns, gateway, subnet);      // config() does not return a value
#endif

  // Observed behavior (NINA Firmware 1.4.8)
  // - at startup connecting sometimes fails with WL_CONNECT_FAILED and error 
  //   reason code 202 (=the authentication fails, but not because of a timeout);
  //   then the second attempt succeeds.
  //   https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/wifi.html#wi-fi-reason-code
  // - after a WiFi signal drop, the code here correctly reconnects, but often 
  //   after a few seconds it needs to reconnect a second time because WiFi status
  //   changes to WL_DISCONNECTED.
  DPRINT(F("Connecting to SSID     : "));
  DPRINTLN(ssid);
  int status = WiFi.begin(ssid, pass);
  if (status == WL_CONNECTED)
  {
    DPRINT(F("Arduino's IP address   : "));
    DPRINTLN(WiFi.localIP());
    DPRINT(F("Gateway's IP address   : "));
    DPRINTLN(WiFi.gatewayIP());
    DPRINT(F("Network's subnet mask  : "));
    DPRINTLN(WiFi.subnetMask());
    return true;
  }
  else
  {
    DPRINT(F("Connection error status: "));
    DPRINTWIFISTATUS(status); DPRINTLN();
    DPRINT(F("Connection error code  : "));
    DPRINTLN(WiFi.reasonCode());
    return false;
  }
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

  // Check for WiFi module/shield
  if (WiFi.status() == WL_NO_MODULE)
  {
    DPRINTLN(F("Communication with WiFi module/shield failed!"));
    while (true);
  }

  // Firmware upgrade available?
  // 1. Upload the example sketch File -> Examples -> WiFiNINA -> Tools -> FirmwareUpdater.
  // 2. Open Tools -> WiFi101 / WiFiNINA Firmware Updater and press "Update Firmware" button.
  String fv = WiFi.firmwareVersion();
  DPRINT(F("Installed firmware     : "));
  DPRINT(fv);
  if (fv < WIFI_FIRMWARE_LATEST_VERSION)
  {  
    DPRINT(F(", please upgrade to "));
    DPRINTLN(WIFI_FIRMWARE_LATEST_VERSION);
  }
  else
    DPRINTLN();

  // Low power mode?
  // The documentation states that the default is WiFi.noLowPowerMode(),
  // but according to my measurements the default is WiFi.lowPowerMode().
  // To avoid inconsistency with future firmware updates, better to 
  // always set the wanted mode explicitly.
  // Nano 33 IoT / MKR WiFi 1010 WiFi.lowPowerMode():     ~50-60mA, client connection latency ~80ms
  // Nano 33 IoT / MKR WiFi 1010 WiFi.noLowPowerMode(): ~100-120mA, client connection latency  ~2ms
  WiFi.noLowPowerMode();
  
  // Connect
  WiFi.setTimeout(connectionTimeoutMs);
  while (!connectToWiFi())
  {
    DPRINT(F("Retrying in            : "));
    DPRINT(connectingRetryMs / 1000);
    DPRINTLN(F(" sec"));
    delay(connectingRetryMs);
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

    // WiFi status poll and reconnect
    // DHCP lease:    the renewal of the DHCP lease is automatically performed by the
    //                NINA module, no need to call a function like Ethernet.maintain().
    // WiFi drop:     normal condition is WiFi.status() WL_CONNECTED, when WiFi drops, 
    //                WiFi.status() first changes to WL_DISCONNECTED and after a while
    //                it becomes WL_CONNECTION_LOST.
    DPRINTLN(F("------------------------------------------"));
    uint8_t wifiStatus = WiFi.status();
    DPRINT(F("WiFi status            : "));
    DPRINTWIFISTATUS(wifiStatus); DPRINTLN();
    DPRINT(F("Signal strength        : "));
    DPRINT(WiFi.RSSI()); DPRINTLN(F(" dBm"));
    DPRINT(F("Client status          : "));
    DPRINTCLIENTSERVERSTATUS(client.status()); DPRINTLN();
    DPRINT(F("Available RAM memory   : "));
    DPRINT(freeMemory()); DPRINTLN(F(" bytes"));
    if (wifiStatus != WL_CONNECTED)
    {
      if (connectToWiFi())
        wifiStatus = WL_CONNECTED; // update it for the below command!
    }

    // Mqtt status poll and reconnect
    bool bDoPublish;
    if (wifiStatus == WL_CONNECTED)
    {
      if (!mqttClient.connected())
        bDoPublish = connectToMqtt();
      else
        bDoPublish = true;
    }
    else
      bDoPublish = false;

    // Publish
    if (bDoPublish)
    {
      int readValue0 = analogRead(A0);
      DPRINT(F("Publish message        : ")); DPRINT(topic0); DPRINT(F("=")); DPRINTLN(readValue0);
      mqttClient.beginMessage(topic0);
      mqttClient.print(readValue0);
      mqttClient.endMessage();
      int readValue1 = analogRead(A1);
      DPRINT(F("Publish message        : ")); DPRINT(topic1); DPRINT(F("=")); DPRINTLN(readValue1);
      mqttClient.beginMessage(topic1);
      mqttClient.print(readValue1);
      mqttClient.endMessage();
      int readValue2 = analogRead(A2);
      DPRINT(F("Publish message        : ")); DPRINT(topic2); DPRINT(F("=")); DPRINTLN(readValue2);
      mqttClient.beginMessage(topic2);
      mqttClient.print(readValue2);
      mqttClient.endMessage();
    }
  }

  // Send MQTT keep alive which avoids being disconnected by the broker
  mqttClient.poll();
}
