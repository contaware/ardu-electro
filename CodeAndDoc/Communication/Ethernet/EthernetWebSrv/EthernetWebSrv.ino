/*
  Ethernet shield / module web server without the use of an SD card
  
  - Most Ethernet shields/modules are 10/100Mbps, they communicate with Arduino
    through SPI. The Ethernet.h library works with W5100/W5200/W5500 based devices
    that provide a network IP stack capable of TCP and UDP. The library supports 
    up to eight (W5100 and boards with <= 2 KB RAM are limited to 4) concurrent 
    connections (incoming, outgoing, or a combination).

  - W5100/W5200/W5500 chips are operated at 3.3V with 5V I/O signal tolerance.
    There are many modules which can either be power by 5V or 3.3V, but pay 
    attention that the original Arduino Ethernet Shield is a 5V only shield.
*/
#include <Ethernet.h>
#include <Dhcp.h>

// For static IP set the define to true and fill the wanted IP in setup()
// For dynamic IP set the define to false
#define USE_STATIC_IP                 true

// If a SDCard reader is present, then set the following to true
// (especially if a card is inserted)
#define DISABLE_SDCARD                true

// Serial Debug
// - if USE_DPRINT is set to true, DPRINT, DPRINTLN, ... do output to Serial Monitor.
// - if USE_DPRINT is set to false, DPRINT, DPRINTLN, ... are optimized away.
#define USE_DPRINT                    true
#define DPRINT_SERIAL_SPEED           9600
#define DPRINT(...)                   do { if (USE_DPRINT) Serial.print(__VA_ARGS__); } while (false)
#define DPRINTLN(...)                 do { if (USE_DPRINT) Serial.println(__VA_ARGS__); } while (false)
#define DWRITE(...)                   do { if (USE_DPRINT) Serial.write(__VA_ARGS__); } while (false)
#define DPRINTLINKSTATUS(...)         do { if (USE_DPRINT) printLinkStatus(__VA_ARGS__); } while (false)
#define DPRINTHARDWARESTATUS(...)     do { if (USE_DPRINT) printHardwareStatus(__VA_ARGS__); } while (false)
#define DPRINTMAINTAINSTATUS(...)     do { if (USE_DPRINT) printMaintainStatus(__VA_ARGS__); } while (false)
// Note: do-while(false) guards against if-else constructs without curly braces.

// Timeouts in ms
const unsigned long startupLinkStatusPollMs = 1000;   // startup link status poll in ms
const unsigned long connectionTimeoutMs = 60000;      // DHCP connection timeout for Ethernet.begin() and Ethernet.maintain(), default is 60 sec
const unsigned long responseTimeoutMs = 4000;         // DHCP response timeout for Ethernet.begin() and Ethernet.maintain(), default is 4 sec
const unsigned long startupConnectingRetryMs = 10000; // do not set under 10 sec, otherwise Ethernet.begin() asks for a DHCP IP too often
const unsigned long ethernetPollMs = 1000;            // do not set above 1 sec so that Ethernet.maintain() can DHCP renew when necessary
unsigned long lastEthernetPollMillis;                 // millis() of the last Ethernet poll

// Ethernet Shields include a sticker with the device's MAC address to set here. If not available,
// choose your own, paying attention that it does not conflict with a MAC address in your LAN.
uint8_t mac[] = {0xA8, 0x61, 0x0A, 0xAE, 0xAB, 0x3A};

// Ethernet SPI SS pin
// 10=Ethernet Shield (default), 5=MKR ETH Shield, 0=Teensy 2.0, 20=Teensy++ 2.0,
// 15=ESP8266 FeatherWing Ethernet, 33=ESP32 FeatherWing Ethernet 
const byte CHOSEN_ETHERNET_SS_PIN = 10;

// SD Card SPI SS pin
// 4=SD on Ethernet Shield / MKR ETH Shield, 10=Adafruit SD shields/modules
// and most Audio shields, 8=Sparkfun SD shield, SDCARD_SS_PIN(28)=MKRZero SD
const byte CHOSEN_SDCARD_SS_PIN = 4;

// Ethernet server
EthernetServer server(80);                            // port 80 is the default for HTTP
const unsigned long clientCloseWaitMs = 1;            // give the web browser time to receive the data
String requestMethod;
String requestURL;
String requestProto;
bool toggleState = false;

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

// Do not call this function directly, only through DPRINTLINKSTATUS
// so that the compiler can optimize it away when USE_DPRINT is set to false
static void printLinkStatus(EthernetLinkStatus ethernetLinkStatus)
{
  // See enum EthernetLinkStatus in Ethernet.h
  // Note: only WIZnet W5200 and W5500 are capable of reporting the
  //       link status, W5100 will report EthernetLinkStatus::Unknown
  switch (ethernetLinkStatus)
  {
    case LinkON:                  Serial.print(F("ON")); break;
    case LinkOFF:                 Serial.print(F("OFF")); break;
    default:                      Serial.print(F("unknown")); break;
  }
}

// Do not call this function directly, only through DPRINTHARDWARESTATUS
// so that the compiler can optimize it away when USE_DPRINT is set to false
static void printHardwareStatus(EthernetHardwareStatus ethernetHardwareStatus)
{
  // See enum EthernetHardwareStatus in Ethernet.h
  switch (ethernetHardwareStatus)
  {
    case EthernetW5100:           Serial.print(F("W5100")); break;
    case EthernetW5200:           Serial.print(F("W5200")); break;
    case EthernetW5500:           Serial.print(F("W5500")); break;
    default:                      Serial.print(F("not found")); break;
  }
}

// Do not call this function directly, only through DPRINTMAINTAINSTATUS
// so that the compiler can optimize it away when USE_DPRINT is set to false
static void printMaintainStatus(int ethernetMaintainStatus)
{
  // See #define in Dhcp.h
  switch (ethernetMaintainStatus)
  {
    case DHCP_CHECK_RENEW_FAIL:   Serial.print(F("renew failed")); break;
    case DHCP_CHECK_RENEW_OK:     Serial.print(F("renew OK")); break;
    case DHCP_CHECK_REBIND_FAIL:  Serial.print(F("rebind failed")); break;
    case DHCP_CHECK_REBIND_OK:    Serial.print(F("rebind OK")); break;
    default:                      Serial.print(F("nothing to do")); break;
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

  // Ethernet SPI SS pin
  Ethernet.init(CHOSEN_ETHERNET_SS_PIN);
  
  // SD Card
#if DISABLE_SDCARD == true
  DPRINTLN(F("SD card reader         : disabled"));
  pinMode(CHOSEN_SDCARD_SS_PIN, OUTPUT);
  digitalWrite(CHOSEN_SDCARD_SS_PIN, HIGH);
#endif

  // Check ethernet cable
  // Note: when starting board + shield sometimes the first call returns OFF
  EthernetLinkStatus ethernetLinkStatus;
  while ((ethernetLinkStatus = Ethernet.linkStatus()) == LinkOFF)
  {
    DPRINT(F("Ethernet link status   : "));
    DPRINTLINKSTATUS(ethernetLinkStatus); DPRINTLN();
    delay(startupLinkStatusPollMs);
  }
  DPRINT(F("Ethernet link status   : "));
  DPRINTLINKSTATUS(ethernetLinkStatus); DPRINTLN();
   
  // Static IP
#if USE_STATIC_IP == true
  IPAddress ip(192, 168, 1, 28);                  // or: byte ip[] = {192, 168, 1, 28};
  IPAddress dns(192, 168, 1, 1);                  // DNS server, optional, defaults to the device IP address with the last byte set to 1
  IPAddress gateway(192, 168, 1, 1);              // network gateway, optional, defaults to the device IP address with the last byte set to 1
  IPAddress subnet(255, 255, 255, 0);             // subnet mask of the network, optional, defaults to 255.255.255.0
  Ethernet.begin(mac, ip, dns, gateway, subnet);  // static IP begin() does not return a value
#else
  // Dynamic IP
  while (Ethernet.begin(mac, connectionTimeoutMs, responseTimeoutMs) != 1)
  {
    DPRINTLN(F("Get IP from DHCP server: failed"));
    DPRINT(F("Retrying in            : "));
    DPRINT(startupConnectingRetryMs / 1000);
    DPRINTLN(F(" sec"));
    delay(startupConnectingRetryMs);
  }
#endif

  // Print detected chip
  DPRINT(F("Ethernet shield        : "));
  DPRINTHARDWARESTATUS(Ethernet.hardwareStatus()); DPRINTLN();

  // Print network details
  DPRINT(F("Arduino's IP address   : "));
  DPRINTLN(Ethernet.localIP());
  DPRINT(F("Gateway's IP address   : "));
  DPRINTLN(Ethernet.gatewayIP());
  DPRINT(F("Network's subnet mask  : "));
  DPRINTLN(Ethernet.subnetMask());
  DPRINT(F("DNS server's IP address: "));
  DPRINTLN(Ethernet.dnsServerIP());

  // Init web server
  server.begin();

  // Init Ethernet poll var
  lastEthernetPollMillis = millis();
}

static bool getQueryValueFromPair(const String& s, int posStartInclusive, int posEndExclusive, const String& param, String& value)
{
  if (posStartInclusive < posEndExclusive)
  {
    // Split at '='
    int pos;
    if ((pos = s.indexOf('=', posStartInclusive)) >= posStartInclusive && pos < posEndExclusive)
    {
      int len = pos - posStartInclusive;
      if (len == (int)param.length() && memcmp(s.c_str() + posStartInclusive, param.c_str(), len) == 0)
      {
        pos++;                      // skip '='
        if (pos < posEndExclusive)  // at least a char?
          value = s.substring(pos, posEndExclusive);
        else
          value = "";
        return true;
      }
    }
    // param only
    else
    {
      int len = posEndExclusive - posStartInclusive;
      if (len == (int)param.length() && memcmp(s.c_str() + posStartInclusive, param.c_str(), len) == 0)
      {
        value = "";
        return true;
      }
    }
  }

  return false;
}
  
static bool getQueryValue(const String& s, const String& param, String& value)
{
  // Search query init
  int pos;
  if ((pos = s.indexOf('?')) < 0)
    return false;
  else
    pos++;
    
  // Loop pairs
  int ampPos;
  while ((ampPos = s.indexOf('&', pos)) >= pos)
  {
    if (getQueryValueFromPair(s, pos, ampPos, param, value))
      return true;
    pos = ampPos + 1; // next pair
  }

  // Last pair
  if (getQueryValueFromPair(s, pos, s.length(), param, value))
    return true;
  
  return false;
}

static void send404NotFound(Client& client)
{
  client.println(F("HTTP/1.1 404 Not Found"));
  client.println(F("Content-Type: text/html; charset=UTF-8"));
  client.println(F("Connection: close")); // the connection will be closed after completion of the response
  client.println();
  client.println(F("<!DOCTYPE html>"));
  client.println(F("<html><head><title>404 Not Found</title></head><body>"));
  client.println(F("<h1>Not Found</h1><p>The requested URL was not found on this server.</p>"));
  client.println(F("</body></html>"));
}

/*
 urlWithQuery: send to the given url which must have at least one query parameter
 functionName: name used for the javascript function which sends asynchronously
 responseID:   if "" nothing is updated, otherwise the HTML element with the given ID
               gets update with xhr.responseText
 disableID:    if "" nothing is disabled, otherwise the HTML element with the given ID
               gets disabled while waiting to the asynchronous answer
 timeoutMs:    if 0 then you have to manually call the javascript functionName function,
               otherwise it is automatically triggered each given milliseconds
*/
static void sendXhr(Client& client, const String& urlWithQuery, const String& functionName, const String& responseID = "", const String& disableID = "", int timeoutMs = 0)
{
  client.print(  F("function ")); client.print(functionName); client.println(F("() {"));
  if (disableID != "")
  {
    client.print(F("  document.getElementById('")); client.print(disableID); client.println(F("').disabled = true;"));
  }
  client.println(F("  const nocache = '&nocache=' + Math.random() * 10;"));
  client.println(F("  const xhr = new XMLHttpRequest();"));
  client.print(  F("  xhr.open('GET', '")); client.print(urlWithQuery); client.println(F("' + nocache, true);"));
  client.println(F("  xhr.onload = function (e) {"));
  client.println(F("    if (xhr.readyState === 4) {"));
  client.println(F("      if (xhr.status === 200) {"));
  if (disableID != "")
  {
    client.print(F("        document.getElementById('")); client.print(disableID); client.println(F("').disabled = false;"));
  }
  if (responseID != "")
  {
    client.print(F("        document.getElementById('")); client.print(responseID); client.println(F("').innerHTML = xhr.responseText;"));
  }
  if (timeoutMs > 0)
  {
    client.print(F("        setTimeout(")); client.print(functionName); client.print(F(", ")); client.print(timeoutMs); client.println(F(");"));
  }
  client.println(F("      }"));
  client.println(F("      else {"));
  if (disableID != "")
  {
    client.print(F("        document.getElementById('")); client.print(disableID); client.println(F("').disabled = false;"));
  }
  if (timeoutMs > 0)
  {
    client.print(F("        setTimeout(")); client.print(functionName); client.print(F(", ")); client.print(timeoutMs); client.println(F(");"));
  }
  client.println(F("      }"));
  client.println(F("    }"));
  client.println(F("  }"));
  client.println(F("  xhr.onerror = function (e) {"));  // no need to output the xhr.status as it is always 0 in case of error
  client.print(  F("    console.error('")); client.print(functionName); client.println(F("() error: connection rejected or network down');"));
  if (disableID != "")
  {
    client.print(F("    document.getElementById('")); client.print(disableID); client.println(F("').disabled = false;"));
  }
  if (timeoutMs > 0)
  {
    client.print(F("    setTimeout(")); client.print(functionName); client.print(F(", ")); client.print(timeoutMs); client.println(F(");"));
  }
  client.println(F("  }"));
  client.println(F("  xhr.send(null);"));
  client.println(F("}"));
  if (timeoutMs > 0)
  {
    client.print(F("window.addEventListener('load', ")); client.print(functionName); client.println(F(");"));
  }
}

void loop()
{
  // Ethernet DHCP maintain and status poll
  unsigned long currentMillis = millis();
  if (currentMillis - lastEthernetPollMillis > ethernetPollMs)
  {
    lastEthernetPollMillis = currentMillis;

    DPRINTLN(F("------------------------------------------"));
    EthernetLinkStatus ethernetLinkStatus = Ethernet.linkStatus();
    DPRINT(F("Ethernet link status   : "));
    DPRINTLINKSTATUS(ethernetLinkStatus); DPRINTLN();
    DPRINT(F("Server status          : "));
    if (server)
      DPRINTLN(F("LISTEN"));
    else
      DPRINTLN(F("NOT LISTENING!")); // usually when the maximum number of sockets is reached
#if USE_STATIC_IP == false
    // Avoid calling Ethernet.maintain() when the link is down, this because
    // Ethernet.maintain() would block for connectionTimeoutMs. As W5100 always 
    // returns Unknown, call Ethernet.maintain() when the status is not LinkOFF:
    if (ethernetLinkStatus != LinkOFF)
    {
      int maintainRet = Ethernet.maintain(); // this function will only re-request a DHCP lease when needed
      DPRINT(F("DHCP maintain status   : "));
      DPRINTMAINTAINSTATUS(maintainRet); DPRINTLN();
      if (maintainRet == DHCP_CHECK_RENEW_OK || maintainRet == DHCP_CHECK_REBIND_OK)
      {
        // By re-requesting a DHCP lease the IP can sometimes change, the http 
        // server continues to work, but it must be accessed with this new IP:
        DPRINT(F("Arduino's IP address   : "));
        DPRINTLN(Ethernet.localIP());
      }
    }
#endif
    DPRINT(F("Available RAM memory   : "));
    DPRINT(freeMemory()); DPRINTLN(F(" bytes"));
  }
  
  // Listen for incoming clients
  EthernetClient client = server.available();
  if (client)
  {
    DPRINTLN(F("------------------------------------------"));
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
          String value;
          if (requestMethod != "GET")
          {
            client.println(F("HTTP/1.1 405 Method Not Allowed"));
            client.println(F("Allow: GET"));
            client.println(F("Content-Type: text/html; charset=UTF-8"));
            client.println(F("Connection: close"));   // the connection will be closed after completion of the response
            client.println();
            if (requestMethod != "HEAD")
            {
              client.println(F("<!DOCTYPE html>"));
              client.println(F("<html><head><title>405 Method Not Allowed</title></head><body>"));
              client.print(  F("<h1>Method Not Allowed</h1><p>The requested method ")); client.print(requestMethod); client.println(F(" is not allowed.</p>"));
              client.println(F("</body></html>"));
            }
          }
          else if (requestURL == "/favicon.ico")      // browsers seek that file to display the little icon on the tab
            send404NotFound(client);
          else if (getQueryValue(requestURL, "status", value))
          {
            client.println(F("HTTP/1.1 200 OK"));
            client.println(F("Content-Type: text/plain; charset=UTF-8"));
            client.println(F("Connection: close"));   // the connection will be closed after completion of the response
            client.println();
            client.println(F(u8"Status 👍")); // UTF-8 symbol
            for (int analogChannel = 0; analogChannel <= 5; analogChannel++)
            {
              client.print(F("analog input "));
              client.print(analogChannel);
              client.print(F(": "));
              client.println(analogRead(analogChannel));
            }
          }
          else if (getQueryValue(requestURL, "toggle", value))
          {
            client.println(F("HTTP/1.1 200 OK"));
            client.println(F("Content-Type: text/plain; charset=UTF-8"));
            client.println(F("Connection: close"));   // the connection will be closed after completion of the response
            client.println();
            toggleState = !toggleState;
            toggleState ? client.println(F("ON")) : client.println(F("OFF"));
          }
          else
          {
            client.println(F("HTTP/1.1 200 OK"));
            client.println(F("Content-Type: text/html; charset=UTF-8"));
            client.println(F("Connection: close"));   // the connection will be closed after completion of the response
            client.println();
            client.println(F("<!DOCTYPE html><html><head><title>Analog Inputs</title><script>"));
            sendXhr(client, "?status", "pollStatus", "status_text", "", 5000);
            sendXhr(client, "?toggle", "toggleState", "toggle_button", "toggle_button");
            client.println(F("</script></head><body>"));
            client.println(F(u8"<h1>💗 Arduino</h1>")); // UTF-8 symbol
            client.println(F("<pre id=\"status_text\">Status loading...</pre>"));
            client.println(F("<br><pre>Toggle button</pre>"));
            client.print(  F("<button id=\"toggle_button\" onclick=\"toggleState()\">"));
            toggleState ? client.print(F("ON")) : client.print(F("OFF"));
            client.println(F("</button>"));
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
  }
}
