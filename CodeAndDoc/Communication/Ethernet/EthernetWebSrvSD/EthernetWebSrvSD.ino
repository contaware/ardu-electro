/*
  Ethernet shield / module web server with the use of an SD card
  
  - The Ethernet shields/modules communicate with Arduino through SPI.
  
  - The Ethernet.h library works with W5100/W5200/W5500 based devices 
    (10BaseT/100BaseTX) that provide a network IP stack capable of TCP and UDP.
    The library supports up to eight (W5100 and boards with <= 2 KB RAM are 
    limited to 4) concurrent connections (incoming, outgoing, or a combination).
  
  - The EthernetENC.h library works with ENC28J60 based devices (10BASE-T),
    which are MAC layer chips requiring a software IP stack provided by the
    library. To avoid too much load the library filters out all broadcasts except
    for the ARP broadcast to which the devices must answer when its IP is queried.
    Some DHCP servers instead of answering through unicast, answer with a brodcast,
    and thus Ethernet.begin(mac) will timeout and fail. In that case either instruct
    your DHCP server to answer with unicast or change the following line in 
    utility/Enc28J60Network.cpp so that broadcasts are not filtered out:
    writeReg(ERXFCON, ERXFCON_UCEN|ERXFCON_CRCEN|ERXFCON_PMEN);
    ->
    writeReg(ERXFCON, ERXFCON_UCEN|ERXFCON_CRCEN|ERXFCON_PMEN|ERXFCON_BCEN);
  
  - W5100/W5200/W5500 chips are operated at 3.3V with 5V I/O signal tolerance.
    Some modules have a 3.3V regulator, thus can be powered by 5V or 3.3V, but
    other ones have no regulator, so they must be powered at 3.3V.
    Pay attention that the original Arduino Ethernet Shield is a 5V only shield.

  - ENC28J60 chip is operated at 3.3V with 5V I/O signal tolerance.
    Some modules have a 3.3V regulator, thus can be powered by 5V or 3.3V, but
    other ones have no regulator, so they must be powered at 3.3V.

  - ENC28J60 loads all images, while W5100/W5200/W5500 sometimes fail to load 
    all images, especially if there are bigger ones. Note that this happens also
    if the concurrent sockets limit is not reached. I suspect that there is a 
    timeout and it would be necessary to server all requests in a timely manner,
    switching to new connections before finishing with the current one.
    
  - The SD.h library works for Micro SD cards that must be formatted as FAT16 or
    FAT32. It uses short 8.3 names for files. The file names passed to the 
    library functions can include paths separated by forward-slashes 
    ("directory/filename.txt"). Because the working directory is always the root
    of the SD card, a name refers to the same file whether or not it includes a
    leading slash ("/file.txt" is equivalent to "file.txt").
*/

// If using the ENC28J60 chip, then set the following to true
#define USE_ENC28J60                  true
#if USE_ENC28J60 == true
#include <EthernetENC.h>
#else
#include <Ethernet.h>
#endif
#include <SD.h>

// For static IP set the define to true and fill the wanted IP in setup()
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
  // Only ENC28J60, WIZnet W5200 and W5500 are capable of reporting the
  // link status, W5100 will report EthernetLinkStatus::Unknown
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
  switch (ethernetHardwareStatus)
  {
#if USE_ENC28J60 == false
    case EthernetW5100:           Serial.print(F("W5100")); break;
    case EthernetW5200:           Serial.print(F("W5200")); break;
    case EthernetW5500:           Serial.print(F("W5500")); break;
#else
    case EthernetENC28J60:        Serial.print(F("ENC28J60")); break;
#endif
    default:                      Serial.print(F("not found")); break;
  }
}

// Do not call this function directly, only through DPRINTMAINTAINSTATUS
// so that the compiler can optimize it away when USE_DPRINT is set to false
#ifndef DHCP_CHECK_NONE
#define DHCP_CHECK_NONE         (0)
#endif
#ifndef DHCP_CHECK_RENEW_FAIL
#define DHCP_CHECK_RENEW_FAIL   (1)
#endif
#ifndef DHCP_CHECK_RENEW_OK
#define DHCP_CHECK_RENEW_OK     (2)
#endif
#ifndef DHCP_CHECK_REBIND_FAIL
#define DHCP_CHECK_REBIND_FAIL  (3)
#endif
#ifndef DHCP_CHECK_REBIND_OK
#define DHCP_CHECK_REBIND_OK    (4)
#endif
static void printMaintainStatus(int ethernetMaintainStatus)
{
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
  if (SD.begin(CHOSEN_SDCARD_SS_PIN))
    DPRINTLN(F("SD card reader         : initialized"));
  else
  {
    DPRINTLN(F("SD card reader         : initialization failed"));
    while (true);
  }

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

static bool sendFile(Client& client, const String& fileName)
{
  File webFile = SD.open(fileName);        
  if (webFile)
  {
    String ext;
    int pos = fileName.lastIndexOf('.');
    if (pos >= 0)
    {
      pos++; // skip '.'
      if (pos < (int)fileName.length())
      {
        ext = fileName.substring(pos);
        ext.toLowerCase();
      }
    }
    client.println(F("HTTP/1.1 200 OK"));
    client.print(F("Content-Length: ")); client.println(webFile.size());
    /*
    RFC-7231 - 3.1.1.5. Content-Type
    A sender that generates a message containing a payload body SHOULD
    generate a Content-Type header field in that message unless the
    intended media type of the enclosed representation is unknown to the
    sender. If a Content-Type header field is not present, the recipient
    MAY either assume a media type of "application/octet-stream"
    ([RFC2046], Section 4.5.1) or examine the data to determine its type.
    */
    if (ext == "htm")
      client.println(F("Content-Type: text/html"));
    else if (ext == "txt")
      client.println(F("Content-Type: text/plain"));
    else if (ext == "css")
      client.println(F("Content-Type: text/css"));
    else if (ext == "ico")
      client.println(F("Content-Type: image/x-icon"));
    else if (ext == "gif")
      client.println(F("Content-Type: image/gif"));
    else if (ext == "png")
      client.println(F("Content-Type: image/png"));
    else if (ext == "jpg")
      client.println(F("Content-Type: image/jpeg"));
    client.println(F("Connection: close")); // the connection will be closed after completion of the response
    client.println();
    uint8_t buf[128]; // for the SD Card the fastest would be 512 bytes
    while (webFile.available())
    {
      int readCount = webFile.read(buf, sizeof(buf));
      client.write(buf, readCount);
    }
    webFile.close();
    return true;
  }
  else
    return false;
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
          else if (requestURL == "/")                 // look after index.htm
          {
            if (!sendFile(client, "index.htm"))
              send404NotFound(client);
          }
          else if (!sendFile(client, requestURL))
            send404NotFound(client);
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
