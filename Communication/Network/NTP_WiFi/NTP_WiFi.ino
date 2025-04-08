/*
  NTP Client over WiFi

  - Get the UTC time from a Network Time Protocol (NTP) time 
    server through UDP packets. NTP uses exclusively the UTC 
    time scale, time zones are not taken into account.

  - NTP supplies a warning of any impending leap second 
    adjustment, and when it needs to insert a leap second, it 
    repeats timestamps for 1 sec. If it needs to remove a leap 
    second, it skips 1 sec of timestamps.

  - There is also the Arduino NTPClient library, but it uses a 
    blocking update() function which sends and then waits for 
    the answer.

  - See: https://en.wikipedia.org/wiki/Network_Time_Protocol
*/
#include <TimeLib.h> // by Michael Margolis, https://github.com/PaulStoffregen/Time
#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_AVR_UNO_WIFI_REV2) || defined(ARDUINO_NANO_RP2040_CONNECT)
  #include <WiFiNINA.h>
#elif defined(ARDUINO_SAMD_MKR1000)
  #include <WiFi101.h>
#elif defined(ARDUINO_ARCH_ESP8266)
  #include <ESP8266WiFi.h>
#elif defined(ARDUINO_PORTENTA_H7_M7) || defined(ARDUINO_NICLA_VISION) || defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_GIGA)
  #include <WiFi.h>
#elif defined(ARDUINO_UNOR4_WIFI)
  #include <WiFiS3.h>
#elif defined(ARDUINO_PORTENTA_C33)
  #include <WiFiC3.h>
#endif
#include <WiFiUdp.h>
#if __has_include("arduino_secrets_gitignored.h")
  #include "arduino_secrets_gitignored.h"         // not required if using the online editor
#else
  #include "arduino_secrets.h"                    // not required if using the online editor
#endif
///////please enter your sensitive data in the Secret tab, arduino_secrets.h or arduino_secrets_gitignored.h
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
// Note: do-while(false) guards against if-else constructs without curly braces.

// Timeouts in ms
const unsigned long wifiStatusPollMs = 100;       // on first connection setup, poll the WiFi status with this rate
const unsigned long connectingRetryMs = 20000;    // do not set under 15 sec for the following reasons:
                                                  // - give WiFi enough time to reconnect
                                                  // - NTP server bans us if we send UDP packets too frequently
unsigned long lastPollMillis;                     // millis() of the last poll
bool reconnectingWiFi = false;

// NTP
const char* timeServerName =          "pool.ntp.org";
#define LOCALUDP_PORT                 2390        // local port to listen for UDP packets (can be changed)
#define TIMESERVER_PORT               123         // that's the standard NTP server port
#define NTP_PACKET_SIZE               48
byte packetBuffer[NTP_PACKET_SIZE];
bool sentNTP = false;
WiFiUDP udp;

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

static void connectToWiFi()
{
  // Disconnect before (re-)connecting
  WiFi.disconnect();

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

  // Connect
  // - At startup the first connection attempt sometimes fails, 
  //   but the second one usually succeeds.
  // - For some platforms WiFi.begin() blocks waiting until it connects
  //   (with a timeout). The code in this sketch works with both behaviors, 
  //   but non-blocking is preferable, thus in setup() the blocking of 
  //   WiFi.begin() is disabled with WiFi.setTimeout(0) for some platforms.
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

static void bindUDP()
{
  // First unbind
  udp.stop();

  // Bind to given local UDP port
  DPRINT(F("Bind to UDP Port       : ")); DPRINTLN(LOCALUDP_PORT);
  if (!udp.begin(LOCALUDP_PORT))
    DPRINTLN(F("UDP bind error         : no sockets available to use"));
}

// This function is needed because some platforms have WiFi.status() 
// returning WL_CONNECTED even if the IP hasn't been received yet
static bool isConnected(uint8_t& status)
{
  status = WiFi.status();
  if (status == WL_CONNECTED)
  {
    IPAddress ip = (IPAddress)WiFi.localIP();
    return ip[0] != 0 || ip[1] != 0 || ip[2] != 0 || ip[3] != 0;
  }
  else
    return false;
}

void setup()
{
#if USE_DPRINT == true
  // Init Serial (leave Serial Monitor open to see all messages)
  Serial.begin(DPRINT_SERIAL_SPEED); delay(5000); // wait 5s that Serial is ready
#endif

#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_AVR_UNO_WIFI_REV2) || defined(ARDUINO_NANO_RP2040_CONNECT) || defined(ARDUINO_UNOR4_WIFI)
  // We want WiFi.begin() to be non-blocking:
  // 1. For ESP8266 and ESP32 it is non-blocking by default.
  // 2. For the defined boards, WiFi.setTimeout(0) makes the trick.
  // 3. For MKR WiFi 1000, WiFi.setTimeout(0) does not work.
  WiFi.setTimeout(0);
#endif

  // Connect to WiFi
  while (true)
  {
    // Connection setup
    lastPollMillis = millis();
    connectToWiFi(); // some platforms have a blocking WiFi.begin(), others a non-blocking

    // Poll till connected
    uint8_t wifiStatus;
    bool hasIP;
    while (!(hasIP = isConnected(wifiStatus)) &&
          (millis() - lastPollMillis) < connectingRetryMs)
    {
      delay(wifiStatusPollMs);
    }

    // Connected?
    if (hasIP)
    {
      DPRINT(F("Arduino's IP address   : "));
      DPRINTLN((IPAddress)WiFi.localIP());    // cast because some libs return uint32_t instead of IPAddress
      DPRINT(F("Gateway's IP address   : "));
      DPRINTLN((IPAddress)WiFi.gatewayIP());  // cast because some libs return uint32_t instead of IPAddress
      DPRINT(F("Network's subnet mask  : "));
      DPRINTLN((IPAddress)WiFi.subnetMask()); // cast because some libs return uint32_t instead of IPAddress
#if !defined(ARDUINO_SAMD_MKR1000)
      DPRINT(F("DNS's IP address       : "));
      DPRINTLN((IPAddress)WiFi.dnsIP());      // cast because some libs return uint32_t instead of IPAddress
#endif
      break;
    }
    else if (wifiStatus == WL_CONNECTED)
      DPRINTLN(F("Connection error status: failed to get an IP address"));
    else
    {
      DPRINT(F("Connection error status: "));
      DPRINTWIFISTATUS(wifiStatus); DPRINTLN();
    }
  }

  // Bind UDP
  bindUDP();
  
  // Init poll var
  lastPollMillis = millis();
}

static void sendNTP()
{
  // Clear flag and flush any existing packets
  sentNTP = false;
  while (udp.parsePacket() != 0)
    udp.flush();

  // Initialize values needed to form the NTP request
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // Send packet requesting timestamp
  DPRINT(F("Sending UDP packet to  : "));
  DPRINT(timeServerName); DPRINT(F(":")); DPRINTLN(TIMESERVER_PORT);
#if USE_DPRINT == true
  unsigned long startMillis = millis();
#endif
  udp.beginPacket(timeServerName, TIMESERVER_PORT);
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  if (udp.endPacket())
    sentNTP = true;
#if USE_DPRINT == true
  unsigned long endMillis = millis();
  DPRINT(F("                         [call time="));
  DPRINT(endMillis - startMillis); DPRINTLN(F(" ms]"));
  if (!sentNTP)
    DPRINTLN(F("UDP sending status     : failed to send the packet"));
#endif
}

static void printTime(time_t t)
{
  int d = day(t);
  int mo = month(t);
  int y = year(t);
  int h = hour(t);
  int m = minute(t);
  int s = second(t);

#if USE_DPRINT == true
  // Date
  if (d < 10) Serial.print(F("0"));
  Serial.print(d);
  Serial.print(F(" "));
  Serial.print(monthShortStr(mo));
  Serial.print(F(" "));
  Serial.print(y);
  Serial.print(F(" "));

  // Time
  if (h < 10) Serial.print(F("0"));
  Serial.print(h);
  Serial.print(F(":"));
  if (m < 10) Serial.print(F("0"));
  Serial.print(m);
  Serial.print(F(":"));
  if (s < 10) Serial.print(F("0"));
  Serial.println(s);
#endif
}

#define SEVENTYYEARS_SEC        2208988800LL
#define NTP_ERA                 0LL
#define SECS_IN_ERA             (UINT32_MAX + 1LL)
static int64_t unixEpochFromNTP(uint32_t ntpTimestamp)
{
  /*
    Do not use time_t as it may be defined as uint32_t, 
    we want int64_t for all the calculations.
  */

  int64_t base = NTP_ERA;
  
  /*
    Once the actual year enters the NTP era 1 (after 2036), increment 
    the NTP_ERA macro by one and remove the following test.
    Once more than half of era 1 has elapsed (after 2104), re-introduce 
    the following test to move to era 2 if ntpTimestamp <= INT32_MAX.
  */
  if (ntpTimestamp <= (uint32_t)INT32_MAX)
    base++;

  return base * SECS_IN_ERA + (int64_t)ntpTimestamp - SEVENTYYEARS_SEC;
}

static void parseNTP()
{
  if (sentNTP)
  {
#if USE_DPRINT == true
    unsigned long startMillis = millis();
#endif

    // Parse
    int packetSize = udp.parsePacket();
    if (packetSize == 0) return;

    // Clear flag
    sentNTP = false;

    // Read the data into the buffer
    udp.read(packetBuffer, NTP_PACKET_SIZE);

#if USE_DPRINT == true
    unsigned long endMillis = millis();
    DPRINT(F("Got UDP packet of      : "));
    DPRINT(packetSize); DPRINTLN(F(" bytes"));
    DPRINT(F("                         [call time="));
    DPRINT(endMillis - startMillis); DPRINTLN(F(" ms]"));
#endif

    // The timestamp starts at byte 40
    uint32_t highWord = word(packetBuffer[40], packetBuffer[41]);
    uint32_t lowWord = word(packetBuffer[42], packetBuffer[43]);
    uint32_t ntpTimestamp = highWord << 16 | lowWord;
    time_t unixTimestamp = (time_t)unixEpochFromNTP(ntpTimestamp);

    // Print times
    DPRINT(F("Current Unix Epoch     : ")); 
    DPRINTLN(unixTimestamp);
    DPRINT(F("Current UTC Time       : ")); 
    printTime(unixTimestamp);
  }
}

void loop()
{
  unsigned long currentMillis = millis();
  if (currentMillis - lastPollMillis > connectingRetryMs)
  {
    lastPollMillis = currentMillis;

    DPRINTLN(F("------------------------------------------"));
    
    // WiFi status
    uint8_t wifiStatus = WiFi.status();
    DPRINT(F("WiFi status            : "));
    DPRINTWIFISTATUS(wifiStatus); DPRINTLN();
    DPRINT(F("Signal strength        : "));
    DPRINT(WiFi.RSSI()); DPRINTLN(F(" dBm"));

    // Reconnect?
    if (!isConnected(wifiStatus))
    {
      connectToWiFi(); // some platforms have a blocking WiFi.begin(), others a non-blocking
      reconnectingWiFi = true;
    }
    else
    {
      if (reconnectingWiFi)
      {
        reconnectingWiFi = false;
        bindUDP(); // re-bind
      }
      sendNTP();
    }
  }

  // Parse NTP packet
  parseNTP();
}
