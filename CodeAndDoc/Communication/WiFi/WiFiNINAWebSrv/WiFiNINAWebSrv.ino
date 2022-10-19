/*
  WiFi Web Server for Arduino MKR WiFi 1010, Arduino MKR VIDOR 4000,
  Arduino UNO WiFi Rev.2 and Nano 33 IoT
  
  - NINA communicates with Arduino through SPI and provides a network IP stack 
    capable of TCP and UDP.

  - NINA can connect either to open or encrypted networks (WEP, WPA).
*/
#include <WiFiNINA.h>

#include "arduino_secrets.h"                      // not required if using the online editor
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
const char ssid[] = SECRET_SSID;                  // your network SSID (name)
const char pass[] = SECRET_PASS;                  // your network password

// For static IP set the define to true and fill the wanted IP in connectToWiFi()
// For dynamic IP set the define to false
#define USE_STATIC_IP                 true

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
unsigned long lastWiFiPollMillis;                 // millis() of the last WiFi poll

// WiFi server
WiFiServer server(80);                            // port 80 is the default for HTTP
const unsigned long clientCloseWaitMs = 1;        // give the web browser time to receive the data
String requestMethod;
String requestURL;
String requestProto;

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

  // Connect
  WiFi.setTimeout(connectionTimeoutMs);
  while (!connectToWiFi())
  {
    DPRINT(F("Retrying in            : "));
    DPRINT(connectingRetryMs / 1000);
    DPRINTLN(F(" sec"));
    delay(connectingRetryMs);
  }

  // Init web server
  server.begin();

  // Init WiFi poll var
  lastWiFiPollMillis = millis();
}

static String parseQueryParam(const String& s, String param)
{
  param += '=';
  String value;
  int pos;
  if ((pos = s.indexOf(param)) >= 0)
  {
    // Point past the equal sign following the param
    pos += param.length();

    // Check whether there is at least a character,
    // if not, it's still a valid empty param
    if (pos < (int)s.length())
    {
      // Point past the value
      int endPos = s.length();
      int ampPos = s.indexOf('&', pos);
      if (ampPos >= 0)
        endPos = ampPos;

      // Get the value
      // (note that if pos == endPos substring() correctly returns the empty string)
      value = s.substring(pos, endPos);
    }
  }
  return value;
}

static void send404NotFound(WiFiClient& client)
{
  client.println(F("HTTP/1.1 404 Not Found"));
  client.println(F("Connection: close")); // the connection will be closed after completion of the response
  client.println();
}

void loop()
{
  // WiFi status poll and reconnect
  // DHCP lease:    the renewal of the DHCP lease is automatically performed by the
  //                NINA module, no need to call a function like Ethernet.maintain().
  // WiFi drop:     normal condition is WiFi.status() WL_CONNECTED, when WiFi drops, 
  //                WiFi.status() first changes to WL_DISCONNECTED and after a while
  //                it becomes WL_CONNECTION_LOST. 
  // Server state:  even if WiFi drops, server.status() remains in LISTEN state.
  unsigned long currentMillis = millis();
  if (currentMillis - lastWiFiPollMillis > connectingRetryMs)
  {
    lastWiFiPollMillis = currentMillis;

    DPRINTLN(F("------------------------------------------"));
    uint8_t wifiStatus = WiFi.status();
    DPRINT(F("WiFi status            : "));
    DPRINTWIFISTATUS(wifiStatus); DPRINTLN();
    DPRINT(F("Signal strength        : "));
    DPRINT(WiFi.RSSI()); DPRINTLN(F(" dBm"));
    DPRINT(F("Server status          : "));
    DPRINTCLIENTSERVERSTATUS(server.status()); DPRINTLN();
    DPRINT(F("Available RAM memory   : "));
    DPRINT(freeMemory()); DPRINTLN(F(" bytes"));
    if (wifiStatus != WL_CONNECTED)
      connectToWiFi();
  }
  
  // Listen for incoming clients
  WiFiClient client = server.available();
  if (client)
  {
    DPRINTLN(F("------------------------------------------"));
    DPRINT(F("Client status          : "));
    DPRINTCLIENTSERVERSTATUS(client.status()); DPRINTLN();
    requestURL = "";
    bool firstLineComplete = false;
    bool currentLineIsBlank = true;
    while (client.connected())  // loop as long as the client is connected
    {
      if (client.available())   // if there are bytes to read from the client
      {
        char c = client.read();
        DWRITE(c);
        if (!firstLineComplete)
          requestURL += c;

        // If we have gotten to the end of the line (received a newline
        // character) and the line is blank, the HTTP request has ended
        // and we can send a reply
        if (c == '\n' && currentLineIsBlank)
        {
          if (requestMethod != "GET")
          {
            client.println(F("HTTP/1.1 405 Method Not Allowed"));
            client.println(F("Allow: GET"));
            client.println(F("Connection: close"));   // the connection will be closed after completion of the response
            client.println();
          }
          else if (requestURL == "/favicon.ico")      // browsers seek that file to display the little icon on the tab
            send404NotFound(client);
          else
          {
            client.println(F("HTTP/1.1 200 OK"));
            client.println(F("Content-Type: text/html; charset=UTF-8"));
            client.println(F("Connection: close"));   // the connection will be closed after completion of the response
            int analogChannel = -1;
            String value = parseQueryParam(requestURL, "channel");
            if (value != "")
            {
              analogChannel = value.toInt();
              if (analogChannel < 0)
                analogChannel = 0;
              else if (analogChannel > 5)
                analogChannel = 5;
              client.println(F("Refresh: 2"));        // refresh the page automatically every 2 sec
            }
            client.println();
            client.println(F("<!DOCTYPE html><html><head><title>Analog Inputs</title></head><body>"));
            client.println(F("Poll analog input: "));
            client.println(F("<a href=\"/?channel=0\">A0</a>"));
            client.println(F("<a href=\"/?channel=1\">A1</a>"));
            client.println(F("<a href=\"/?channel=2\">A2</a>"));
            client.println(F("<a href=\"/?channel=3\">A3</a>"));
            client.println(F("<a href=\"/?channel=4\">A4</a>"));
            client.println(F("<a href=\"/?channel=5\">A5</a>"));
            if (analogChannel != -1)
            {
              client.print(F("<br><br>&rarr; analog input "));
              client.print(analogChannel);
              client.print(F(" is "));
              client.println(analogRead(analogChannel));
              client.println(F(u8"<br><br><a href=\"/\">Stop Poll</a> and 😎 relax")); // UTF-8 smile :-)
            }
            client.println(F("</body></html>"));
          }
          break;                  // exit the while loop and jump to the below close connection command 
        }
        else if (c == '\n')       // starting a new line
        {
          currentLineIsBlank = true;
          if (!firstLineComplete)
          {
            firstLineComplete = true;
            
            // Parse request line
            requestURL.trim(); // remove ending CRLF
            int pos = requestURL.indexOf(' ');
            requestMethod = requestURL.substring(0, pos);
            requestURL.remove(0, pos + 1);
            pos = requestURL.lastIndexOf(' ');
            requestProto = requestURL.substring(pos + 1);
            requestURL.remove(pos);
          }
        }
        else if (c != '\r')       // we have gotten a character on the current line
          currentLineIsBlank = false;
      }
    } 
    delay(clientCloseWaitMs);     // give the web browser time to receive the data
    client.stop();                // close the connection
    DPRINT(F("Client status          : "));
    DPRINTCLIENTSERVERSTATUS(client.status()); DPRINTLN();
  }
}
