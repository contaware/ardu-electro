/*
  Ethernet shields or modules (some have a SD card reader)
  
  - Most Ethernet shields/modules are 10/100Mbps, they communicate with Arduino
    through SPI. The Ethernet.h library works with W5100/W5200/W5500 based devices
    that provide a network IP stack capable of TCP and UDP. The library supports 
    up to eight (W5100 and boards with <= 2 KB RAM are limited to 4) concurrent 
    connections (incoming, outgoing, or a combination).

  - There is also an official shield variant with a Power over Ethernet (PoE)
    module soldered on the shield.

  - The SD.h library works for Micro SD cards that must be formatted as FAT16 or
    FAT32. It uses short 8.3 names for files. The file names passed to the 
    library functions can include paths separated by forward-slashes 
    ("directory/filename.txt"). Because the working directory is always the root
    of the SD card, a name refers to the same file whether or not it includes a
    leading slash ("/file.txt" is equivalent to "file.txt").
*/
#include <Ethernet.h>

// For static IP set it to 1
// For dynamic IP set it to 0
#define USE_STATIC_IP           1

// To use the SDCard and print its index.htm file set it to 1
// To display the Arduino analog inputs set it to 0
#define USE_SDCARD              0

// Serial Debug
// - if USE_DPRINT is set to true, DPRINT, DPRINTLN and DWRITE do output to Serial Monitor.
// - if USE_DPRINT is set to false, DPRINT, DPRINTLN and DWRITE are optimized away.
#define USE_DPRINT              true
#define DPRINT_SERIAL_SPEED     9600
#define DPRINT(...)             do { if (USE_DPRINT) Serial.print(__VA_ARGS__); } while (false)
#define DPRINTLN(...)           do { if (USE_DPRINT) Serial.println(__VA_ARGS__); } while (false)
#define DWRITE(...)             do { if (USE_DPRINT) Serial.write(__VA_ARGS__); } while (false)
// Note: do-while(false) guards against if-else constructs without curly braces.

// Ethernet Shields include a sticker with the device's MAC address to set here. If not available,
// choose your own, paying attention that it does not conflict with a MAC address in your LAN.
const byte mac[] = {0xA8, 0x61, 0x0A, 0xAE, 0xAB, 0x3A};

// Ethernet SPI SS pin
// 10=Ethernet Shield (default), 5=MKR ETH Shield, 0=Teensy 2.0, 20=Teensy++ 2.0,
// 15=ESP8266 FeatherWing Ethernet, 33=ESP32 FeatherWing Ethernet 
const byte CHOSEN_ETHERNET_SS_PIN = 10;

// SD Card SPI SS pin
// 4=SD Card reader on Ethernet Shield
const byte CHOSEN_SDCARD_SS_PIN = 4;

// SD Card
#if USE_SDCARD == 1
#include <SD.h>
File webFile;
#endif

// Initialize the Ethernet server library
EthernetServer server(80); // port 80 is the default for HTTP

// Received HTTP request (split in method, url and protocol)
String requestMethod;
String requestURL;
String requestProto;

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
#if USE_SDCARD == 1
  if (SD.begin(CHOSEN_SDCARD_SS_PIN))
    DPRINTLN(F("SD card initialized"));
  else
  {
    DPRINTLN(F("SD card initialization failed!"));
    while (true);
  }
#else
  // Note that if you do not use the SD card reader but
  // a card is inserted, then please remove it or make
  // sure to always execute the following code:
  DPRINTLN(F("Disabling SD card reader"));
  pinMode(CHOSEN_SDCARD_SS_PIN, OUTPUT);
  digitalWrite(CHOSEN_SDCARD_SS_PIN, HIGH);
#endif

  // Check ethernet cable
  // Note: only WIZnet W5200 and W5500 are capable of reporting
  //       the link status, W5100 will report "Unknown"
  byte linkCheckCount = 0;
  while (Ethernet.linkStatus() == LinkOFF)
  {
    if ((++linkCheckCount % 4) == 0)
      DPRINTLN(F("Link OFF, please plug-in the ethernet cable!"));
    delay(500);
  }
   
  // Static IP
#if USE_STATIC_IP == 1
  IPAddress ip(192, 168, 1, 28);              // or: byte ip[] = {192, 168, 1, 28};
  IPAddress dns(192, 168, 1, 1);              // DNS server, optional, defaults to the device IP address with the last byte set to 1
  IPAddress gateway(192, 168, 1, 1);          // network gateway, optional, defaults to the device IP address with the last byte set to 1
  IPAddress subnet(255, 255, 255, 0);         // subnet mask of the network, optional, defaults to 255.255.255.0
  Ethernet.begin(mac, ip, dns, gateway, subnet); // the static begin() versions don't return a value
#else
  // Dynamic IP
  const unsigned long timeout = 60000;        // optional, defaults to 60000 ms
  const unsigned long responseTimeout = 4000; // optional, defaults to 4000 ms
  if (Ethernet.begin(mac, timeout, responseTimeout) == 0) // returns 1 on success and 0 on failure
  {
    DPRINTLN(F("Failed to obtaining an IP address"));
    while (true);
  }
#endif

  // Print detected chip
  if (Ethernet.hardwareStatus() == EthernetNoHardware)
    DPRINTLN(F("Ethernet shield was not found"));
  else if (Ethernet.hardwareStatus() == EthernetW5100)
    DPRINTLN(F("W5100 Ethernet controller detected"));
  else if (Ethernet.hardwareStatus() == EthernetW5200)
    DPRINTLN(F("W5200 Ethernet controller detected"));
  else if (Ethernet.hardwareStatus() == EthernetW5500)
    DPRINTLN(F("W5500 Ethernet controller detected"));

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
    if (pos < s.length())
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

static void send404NotFound(EthernetClient& client)
{
  client.println(F("HTTP/1.1 404 Not Found"));
  client.println(F("Connection: close")); // the connection will be closed after completion of the response
  client.println();
}
              
void loop()
{
  // Listen for incoming clients
  EthernetClient client = server.available();
  if (client)
  {
    requestURL = "";
    bool firstLineComplete = false;
    bool currentLineIsBlank = true;
    while (client.connected())
    {
      if (client.available())
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
          {
#if USE_SDCARD == 1
            webFile = SD.open("favicon.ico");        
            if (webFile)
            {
              client.println(F("HTTP/1.1 200 OK"));
              client.print(F("Content-Length: ")); client.println(webFile.size());
              client.println(F("Content-Type: image/x-icon"));
              client.println(F("Connection: close")); // the connection will be closed after completion of the response
              client.println();
              while (webFile.available())
                client.write(webFile.read());
              webFile.close();
            }
            else
              send404NotFound(client);
#else
            send404NotFound(client);
#endif
          }
          else
          {
#if USE_SDCARD == 1
            webFile = SD.open("index.htm");        
            if (webFile)
            {
              client.println(F("HTTP/1.1 200 OK"));
              client.println(F("Content-Type: text/html"));
              client.println(F("Connection: close")); // the connection will be closed after completion of the response
              client.println();
              while (webFile.available())
                client.write(webFile.read());
              webFile.close();
            }
            else
              send404NotFound(client);
#else
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
#endif    
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
    delay(1);                     // give the web browser time to receive the data
    client.stop();                // close the connection
  }
}
