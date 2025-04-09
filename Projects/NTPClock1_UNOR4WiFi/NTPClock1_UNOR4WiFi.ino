/*
  5050 RGB led (with WS2812B chip) matrix to display a clock 
  and the temperature with a UNO R4 WiFi
  
  - Each LED can use up to 60mA, make your calcs and power the 
    LED matrix with an external 5V supply.
  
  - As the Arduino UNO R4 RTC is not so accurate, we connect 
    often to the NTP server to correct it.

  - The used temperature/humidity sensor is the well known 
    DHT22 (=AM2302).
*/
#include <Adafruit_GFX.h>
#include <Framebuffer_GFX.h>       // by Marc Merlin
#include <FastLED_NeoMatrix.h>     // by Marc Merlin
#include <FastLED.h>               // by Daniel Garcia
#include <RTC.h>
#include <TimeLib.h>               // by Michael Margolis, https://github.com/PaulStoffregen/Time
#include <Timezone.h>              // by Jack Christensen, https://github.com/JChristensen/Timezone
#include <SimpleDHT.h>             // by Winlin
#include <WiFiS3.h>
#include <WiFiUdp.h>
#if __has_include("arduino_secrets_gitignored.h")
  #include "arduino_secrets_gitignored.h" // not required if using the online editor
#else
  #include "arduino_secrets.h"            // not required if using the online editor
#endif
///////please enter your sensitive data in the Secret tab, arduino_secrets.h or arduino_secrets_gitignored.h
const char ssid[] = SECRET_SSID;          // your network SSID (name)
const char pass[] = SECRET_PASS;          // your network password

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

#define DHT_PIN                 3  // Temp and humidity sensor PIN
#define LED_PIN                 2  // LED matrix PIN
#define MATRIX_WIDTH            32 // width of single matrix
#define MATRIX_HEIGHT           8  // height of single matrix
#define TILESX                  3  // how many tiles in X direction
#define TILESY                  1  // how many tiles in Y direction
#define NUM_LEDS                TILESX*TILESY*MATRIX_WIDTH*MATRIX_HEIGHT
CRGB leds[NUM_LEDS];

// LED Matrix
FastLED_NeoMatrix matrix = FastLED_NeoMatrix(
  leds,
  MATRIX_WIDTH,
  MATRIX_HEIGHT,
  TILESX,
  TILESY,
  NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG +
  NEO_TILE_TOP + NEO_TILE_LEFT + NEO_TILE_ROWS + NEO_TILE_PROGRESSIVE);
const uint16_t dayMonthColor = matrix.Color(206, 191, 55);
const uint16_t timeColor = matrix.Color(255, 255, 255);
const uint16_t tempColdColor = matrix.Color(13, 167, 209);
const uint16_t tempWarmColor = matrix.Color(161, 228, 25);
const uint16_t tempHotColor = matrix.Color(255, 131, 4);
const uint16_t tempErrorColor = matrix.Color(255, 0, 0);

// Temperature/humidity sensor
SimpleDHT22 dht22(DHT_PIN);

// WiFi and NTP
const unsigned long connectingRetryMs = 20000; // do not set under 15 sec for the following reason:
                                               // - give WiFi enough time to reconnect
unsigned long lastWiFiPollMillis;              // millis() of the last WiFi poll
bool reconnectingWiFi = false;
const int ntpUpdateRate = 30;                  // NTP queries rate = ntpUpdateRate * connectingRetryMs
int ntpSendCountdown;                          // when reaching 0, a NTP packet is sent
const char* timeServerName =          "pool.ntp.org";
#define LOCALUDP_PORT                 2390     // local port to listen for UDP packets (can be changed)
#define TIMESERVER_PORT               123      // that's the standard NTP server port
#define NTP_PACKET_SIZE               48
byte packetBuffer[NTP_PACKET_SIZE];
bool sentNTP = false;
WiFiUDP udp;

// Time
time_t lastUtcTimestampFromNTPtoRTC = 0;       // last set NTP timestamp, 0 means no NTP time yet
const unsigned long rtcPollRateMs = 1000;      // do not set under 1 sec
unsigned long lastRTCPollMillis;               // millis() of the last poll
int prevMinute = -1;                           // last displayed minute, -1 forces display
// Set DST start and end times (Central Europe example)
// DST starts 01:00 UTC (02:00 CET) on the last Sunday of March
// DST ends 01:00 UTC (03:00 CEST) on the last Sunday of October
TimeChangeRule myDSTStart = {"CEST", Last, Sun, Mar, 2, 120}; // Daylight time = UTC + 2 hours
TimeChangeRule mySTDStart = {"CET", Last, Sun, Oct, 3, 60};   // Standard time = UTC + 1 hour
Timezone myTZ(myDSTStart, mySTDStart);
#define SECONDS_FROM_1970_TO_2000    946684800 // earliest supported time (unixtime for 2000-01-01 00:00:00)
#define MY_SHORT_MONTH_STRLEN        3         // the length of the short month string
static char myBuffer[MY_SHORT_MONTH_STRLEN + 1];
const char myMonthShortNames_P[] PROGMEM = "errgenfebmaraprmaggiulugagosetottnovdic";
char* myMonthShortStr(uint8_t month)
{
  for (int i = 0 ; i < MY_SHORT_MONTH_STRLEN ; i++)
    myBuffer[i] = pgm_read_byte(&(myMonthShortNames_P[i + (month*MY_SHORT_MONTH_STRLEN)]));
  myBuffer[MY_SHORT_MONTH_STRLEN] = 0;
  return myBuffer;
}

static void connectToWiFi()
{
  // Disconnect before (re-)connecting
  WiFi.disconnect();

  // Connect
  // - At startup the first connection attempt sometimes fails, 
  //   but the second one usually succeeds.
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

// This function is needed because WiFi.status() may return 
// WL_CONNECTED even if the IP hasn't been received yet
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

  // We want WiFi.begin() to be non-blocking
  WiFi.setTimeout(0);

  // Init LED matrix
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS); // color order: RGB, RBG, GRB, GBR, BRG, BGR
  matrix.begin();
  matrix.cp437(true);       // use the correct CP437 character codes
  matrix.setTextWrap(false);
  matrix.setTextSize(1);    // 1 is the default 6x8, 2 is 12x16, 3 is 18x24, etc
  matrix.setRotation(0);    // 0 thru 3 corresponding to 4 cardinal rotations
  matrix.setBrightness(10); // 0..255

  // Init RTC with the earliest supported time,
  // NTP will later on set the correct time.
  RTC.begin();
  RTCTime utcRTCTime((time_t)SECONDS_FROM_1970_TO_2000);
  RTC.setTime(utcRTCTime);

  // Init poll vars (those unsigned long vars correctly wrap-back)
  unsigned long currentMillis = millis();
  lastWiFiPollMillis = currentMillis - connectingRetryMs;
  lastRTCPollMillis = currentMillis - rtcPollRateMs;
  ntpSendCountdown = 1;
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

static void display(bool rtcAccurate, time_t t, bool tempOk, float temp)
{
  // Turn off all pixels (pixel in 16-bit '565' RGB format)
  // and reset cursor
  matrix.fillScreen(0);
  matrix.setCursor(0, 0);

  // Day and Month
  matrix.setTextColor(dayMonthColor);
  int d = day(t);
  if (d < 10) matrix.print("0");
  matrix.print(d);
  matrix.print(myMonthShortStr(month(t))); // for english use lib's monthShortStr()
  matrix.print(" ");

  // Time
  int h = hour(t);
  int m = minute(t);
  matrix.setTextColor(timeColor);
  if (rtcAccurate)
  {
    if (h < 10) matrix.print("0");
    matrix.print(h);
    matrix.print(":");
    if (m < 10) matrix.print("0");
    matrix.print(m);
  }
  else
    matrix.print("--:--");
  matrix.print(" ");

  // Temperature
  if (tempOk)
  {
    long tempLong = (long)roundf(temp);
    if (tempLong < 15)
      matrix.setTextColor(tempColdColor);
    else if (tempLong < 25)
      matrix.setTextColor(tempWarmColor);
    else
      matrix.setTextColor(tempHotColor);
    if (tempLong >= 0) matrix.print("+");
    if (tempLong < 10) matrix.print("0");
    matrix.print(tempLong);
  }
  else
  {
    matrix.setTextColor(tempErrorColor);
    matrix.print("+xx");
  }
  matrix.write(0xF8); // print the degrees symbol

  // Show text
  matrix.show();

  // Serial debug
#if USE_DPRINT == true
  if (rtcAccurate)
    Serial.print(F("RTC accurate           : "));
  else
    Serial.print(F("RTC NOT accurate       : "));
  if (d < 10) Serial.print(F("0"));
  Serial.print(d);
  Serial.print(F(" "));
  Serial.print(myMonthShortStr(month(t))); // for english use lib's monthShortStr()
  Serial.print(F(" "));
  Serial.print(year(t));
  Serial.print(F(" "));
  if (h < 10) Serial.print(F("0"));
  Serial.print(h);
  Serial.print(F(":"));
  if (m < 10) Serial.print(F("0"));
  Serial.print(m);
  Serial.print(F(":"));
  int s = second(t);
  if (s < 10) Serial.print(F("0"));
  Serial.print(s);
  if (tempOk)
  {
    Serial.print(F(" (temp="));
    Serial.print(temp, 1);
    Serial.println(F("°C)"));
  }
  else
    Serial.println(F(" (temp read error!)"));
#endif
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

    // Update RTC
    time_t utcTimestamp = (time_t)unixEpochFromNTP(ntpTimestamp);
    RTCTime utcRTCTime;
    RTC.getTime(utcRTCTime);
    time_t oldUtcTimestamp = utcRTCTime.getUnixTime(); // store current
    utcRTCTime.setUnixTime(utcTimestamp);
    if (RTC.setTime(utcRTCTime))
    {
      if (lastUtcTimestampFromNTPtoRTC == 0)
      {
        DPRINTLN(F("First update of RTC with NTP"));
        prevMinute = -1; // force display
      }
      else
      {
        DPRINT(F("Updated RTC with NTP   : correction="));
        DPRINT((long)(oldUtcTimestamp - utcTimestamp));
        DPRINTLN(F(" sec"));
      }
      lastUtcTimestampFromNTPtoRTC = utcTimestamp;
    }
    else
      DPRINTLN(F("Failed to updated RTC with NTP!"));
  }
}

void loop()
{
  unsigned long currentMillis = millis();

  // WiFi connection check
  if (currentMillis - lastWiFiPollMillis > connectingRetryMs)
  {
    lastWiFiPollMillis = currentMillis;

    // (Re-)connect?
    uint8_t wifiStatus;
    if (!isConnected(wifiStatus))
    {
      connectToWiFi(); // non-blocking
      reconnectingWiFi = true;
      ntpSendCountdown = 1;
    }
    else
    {
      if (reconnectingWiFi)
      {
        reconnectingWiFi = false;
        bindUDP(); // re-bind
      }
      if (--ntpSendCountdown <= 0)
      {
        ntpSendCountdown = ntpUpdateRate;
        sendNTP();
      }
    }
  }

  // Parse NTP packet
  parseNTP();
  
  // Poll RTC and each minute update matrix text
  if (currentMillis - lastRTCPollMillis > rtcPollRateMs)
  {
    lastRTCPollMillis = currentMillis;

    // Get UTC time and convert to local time
    RTCTime utcRTCTime;
    RTC.getTime(utcRTCTime);
    time_t utcTimestamp = utcRTCTime.getUnixTime();
    const time_t rtcAccuracyThreshold = 3 * ntpUpdateRate * connectingRetryMs / 1000;
    bool rtcAccurate = (utcTimestamp - lastUtcTimestampFromNTPtoRTC) < rtcAccuracyThreshold;
    time_t localTimestamp = myTZ.toLocal(utcTimestamp);
    int m = minute(localTimestamp);
    if (m != prevMinute &&      // only display in case of a new minute but
        (m+1)%60 != prevMinute) // avoid showing jump-backs which can happen with NTP adjustments
    {
      prevMinute = m;

      // Read temperature (we do not display the humidity)
      // Note: DHT22 sampling rate is 0.5 HZ
      //       -> be above that otherwise an error is returned!
      float temp, humidity;
      bool tempOk = dht22.read2(&temp, &humidity, NULL) == SimpleDHTErrSuccess;

      // Update text on LED matrix
      display(rtcAccurate, localTimestamp, tempOk, temp);
    }
  }
}
