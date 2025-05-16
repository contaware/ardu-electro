/*
  Ethernet shield / module client request

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
*/
#include <FreeStack.h>   // install SdFat library by Bill Greiman 
                         // https://github.com/greiman/SdFat

// If using the ENC28J60 chip, then set the following to true
#define USE_ENC28J60                  false
#if USE_ENC28J60 == true
#include <EthernetENC.h> // by Norbert Truchsess, Juraj Andrassy
#else
#include <Ethernet.h>    // Ethernet by Various
#endif

// For static IP set the define to true and fill the wanted IP in setup()
// For dynamic IP set the define to false
#define USE_STATIC_IP                 false

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
const unsigned long clientRequestMs = 15000;          // do a client request each given ms
unsigned long lastClientRequestMillis;                // millis() of the last Client request

// Ethernet Shields include a sticker with the device's MAC address to set here. If not available,
// choose your own, paying attention that it does not conflict with a MAC address in your LAN.
uint8_t mac[] = {0xA8, 0x61, 0x0A, 0xAE, 0xAB, 0x3A};

// Ethernet SPI CS pin
// 10=Ethernet Shield, 5=MKR ETH Shield
const byte CHOSEN_ETHERNET_CS_PIN = 10;

// SD Card SPI CS pin
// 4=SD on Ethernet Shield / MKR ETH Shield, 10=Adafruit SD shields/modules
// and most Audio shields, 8=Sparkfun SD shield, SDCARD_SS_PIN(28)=MKRZero SD
const byte CHOSEN_SDCARD_CS_PIN = 4;

// Ethernet client
#define CONNECT_PORT                  80
EthernetClient client;

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
#if USE_DPRINT == true
  // Init Serial (leave Serial Monitor open to see all messages)
  Serial.begin(DPRINT_SERIAL_SPEED); delay(5000); // wait 5s that Serial is ready
#endif

  // Ethernet SPI CS pin
  Ethernet.init(CHOSEN_ETHERNET_CS_PIN);
  
  // SD Card
#if DISABLE_SDCARD == true
  DPRINTLN(F("SD card reader         : disabled"));
  pinMode(CHOSEN_SDCARD_CS_PIN, OUTPUT);
  digitalWrite(CHOSEN_SDCARD_CS_PIN, HIGH);
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

  // Init poll vars
  lastEthernetPollMillis = lastClientRequestMillis = millis();
}

// Test client connection
void testClient(const char* host, const char* reqURI)
{
  DPRINTLN(F("------------------------------------------"));

  // Disconnect if still connected
  if (client.connected())
  {
    DPRINTLN(F("Client closes the connection."));
    client.stop();
  }

  // Connect
  DPRINT(F("Connecting to          : "));
  DPRINT(host); DPRINT(F(":")); DPRINTLN(CONNECT_PORT);
  if (!client.connect(host, CONNECT_PORT))
  {
    DPRINTLN(F("Connection failed!"));
    return;
  }

  // Send GET request and tell the server that the connection 
  // can be closed. If the server does not close it, we will 
  // do that at the start of this function with the next call.
  client.print(F("GET ")); client.print(reqURI); client.println(F(" HTTP/1.1"));
  client.print(F("Host: ")); client.println(host);
  client.println(F("Connection: close"));
  client.println();
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
    DPRINT(F("Client status          : "));
    if (client.connected())
      DPRINTLN(F("CONNECTED"));
    else
      DPRINTLN(F("NOT CONNECTED"));
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
#if USE_DPRINT == true
    int freeMem = FreeStack();
    // For unsupported platforms it returns 0,
    // and some unsupported ARMs return a negative number
    if (freeMem > 0)
    { 
      DPRINT(F("FREE memory            : "));
      DPRINT(freeMem); DPRINTLN(F(" bytes"));
    }
#endif
  }

  // Do client request
  if (currentMillis - lastClientRequestMillis > clientRequestMs)
  {
    lastClientRequestMillis = currentMillis;

    testClient("httpbun.com", "/ip.txt");
    //testClient("httpbun.com", "/headers");
    //testClient("example.com", "/");
  }

  // Print response
  if (client.connected() && client.available())
  {
    DPRINTLN(F("------------------------------------------"));

    while (client.available())
      Serial.write(client.read());

    // As we do not parse the response and thus we do not check the Content-Length, 
    // some more data could reach us later on. For this reason we let the server 
    // close the connection. Note: if for some reasons the connection remains open,
    // in testClient() we close it.

    // Print a space because text responses may not have a newline at the end 
    DPRINTLN();
  }
}
