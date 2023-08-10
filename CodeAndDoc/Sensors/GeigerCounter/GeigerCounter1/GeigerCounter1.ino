/*
  Full project of a Geiger counter for beta and gamma radiations,
  based on RadiationD-v1.1 (CAJOE) and using an oled display with
  a rotary encoder to control the UI 

  **** ATTENTION: THERE ARE UP TO 500 VDC ON THE TUBE ****

  - VDD supply is 5V, used current is 12mA - 30mA. 
         
  - This detector can drive M4011, STS-5, SBM20 or J305 Geiger Müller
    tubes. Has voltage compatibility with all popular GM tubes that
    require an anode voltage between 350VDC - 500VDC.

  - Removing jumper J1 switches off the buzzer.
  
  - Jumper J4 is used for calibration.

  - There is a sliding switch which interrupts the 5V from the DC power
    connector and from the nearby terminal block. Note that the 5V pin
    close to the VIN/INT pin is always on.
    
  - There are 3 pins to connect to an Arduino: VIN/INT, GND and 5V.
    The kit sends nice and clean 400 uS high-low-high pulses with a 
    high impedance (470k). We can connect it directly to a 3.3V 
    Arduino because it has such a high impedance, but if you want
    to be really safe, add a 1 MΩ resistor from VIN/INT to ground
    to create a voltage divider.

  - The GM tube M4011 conversion index is 151, which means that
    151 CPM = 1μSv/h. The M4011 tube has an own background of around
    25 CPM.
    
  - 0.274 μSv/h = 2.4 mSv/year        : Worldwide avg natural dose
    5.7-11.4 μSv/h = 50-100 mSv/year  : Max yearly dose before health damage 
    100 μSv                           : The radiation during a x-ray
*/
#include <limits.h>
#include <SimplyAtomic.h>
#include "RotaryEncoderPoll.h"
#include "ButtonPoll.h"
#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include "arduino_secrets.h"                  // not required if using the online editor
///////please enter your sensitive data in the Secret tab/arduino_secrets.h

// Counter
const unsigned long SAMPLE_PERIOD_MS = 20000; // Do not set under 15 sec for the following reasons:
                                              // - Geiger counter is less precise if counting for a shorter period
                                              // - reconnects would end-up to be too frequent (for both WiFi and Broker)
                                              // - mqtt3.thingspeak.com requires at least 15 sec between published messages
const byte DET_PIN = 2;
unsigned long nowCPM = 0;
unsigned long avgCPM = 0;
unsigned long minCPM = ULONG_MAX;
unsigned long maxCPM = 0;
volatile unsigned long count = 0;
unsigned long previousMillis;

// Rotary encoder with button
const byte ROTARY_ENCODER_CLK_PIN = 8;
const byte ROTARY_ENCODER_DT_PIN = 9;
const byte ROTARY_ENCODER_SW_PIN = 7;
RotaryEncoderPoll enc;
ButtonPoll btn;

// WiFi and Mqtt client
const char ssid[] = SECRET_SSID;              // your network SSID (name)
const char pass[] = SECRET_PASS;              // your network password
uint8_t wifiStatus = WL_DISCONNECTED;
WiFiSSLClient client;
MqttClient mqttClient(client);
uint8_t mqttClientConnected = 0;
int mqttEndMessageRet = 0;
const char broker[] = "mqtt3.thingspeak.com";
int        port     = 8883;
const char topic[]  = SECRET_MQTT_TOPIC;

// Oled
#define SCREEN_WIDTH          128             // OLED display width, in pixels, usually 128
#define SCREEN_HEIGHT         64              // OLED display height, in pixels, usually 64 or 32
#define SCREEN_ADDRESS        0x3C            // see board for Address: 0x3C or 0x3D
#define SH1106_STARTUP_MS     500             // SH1106 needs a small amount of time to be ready after initial power
int displayPage = 0;
const int DISPLAY_PAGES = 2;
Adafruit_SH1106G oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); // for STEMMA QT the RST pin is not necessary, so we pass -1

void detectedISR()
{
  count++;
}

void setup()
{
  // Counter
  pinMode(DET_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(DET_PIN), detectedISR, FALLING);
  previousMillis = millis();

  // Init rotary encoder with button
  enc.begin(ROTARY_ENCODER_CLK_PIN, ROTARY_ENCODER_DT_PIN);
  btn.begin(ROTARY_ENCODER_SW_PIN);

  // Init MQTT
  mqttClient.setId(SECRET_MQTT_CLIENT_ID);
  mqttClient.setUsernamePassword(SECRET_MQTT_USERNAME, SECRET_MQTT_PASSWORD);
  
  /*
    Oled
    When powering the device if your code tries to write to the display too soon,
    it just shows a black screen (oled.begin() succeeds but nothing works).
    Note that it will work on reset since that typically does not cycle power. 
    See: https://learn.adafruit.com/monochrome-oled-breakouts/troubleshooting-2
  */
  delay(SH1106_STARTUP_MS);
  oled.begin(SCREEN_ADDRESS, true);
  oled.clearDisplay();
  oled.cp437(true);                           // use the 'Code Page 437'-compatible charset
  oled.setTextColor(SH110X_WHITE);            // draw white text
  oled.setTextSize(2);                        // draw 2X-scale text
  int16_t x1, y1;
  uint16_t w, h;
  const char geigerStr[] = "Geiger";
  oled.getTextBounds(geigerStr, 0, 0, &x1, &y1, &w, &h);
  oled.setCursor((oled.width() - w) / 2, 2);
  oled.println(geigerStr);
  const char counterStr[] = "Counter"; 
  oled.getTextBounds(counterStr, 0, 0, &x1, &y1, &w, &h);
  oled.setCursor((oled.width() - w) / 2, oled.getCursorY());
  oled.println(counterStr);
  const char pleaseWaitStr[] = "please wait...";
  oled.setTextSize(1);                        // restore default 1:1 pixel scale
  oled.getTextBounds(pleaseWaitStr, 0, 0, &x1, &y1, &w, &h);
  oled.setCursor((oled.width() - w) / 2, oled.getCursorY() + 12);
  oled.print(pleaseWaitStr);
  oled.display();
}

void displayCurrentPage()
{
  oled.clearDisplay();
  oled.setCursor(0, 0);
  if (displayPage == 0)
  {
    oled.setTextSize(2);                      // draw 2X-scale text
    oled.print("CPM ");
    oled.println(nowCPM);
    oled.setTextSize(1);                      // restore default 1:1 pixel scale
    const char unitStr[] = {' ',230,'S','v','/','h','\0'};
    oled.print("\nNow ");
    oled.print((float)nowCPM / 151.0);        // to μSv/h
    oled.println(unitStr);
    oled.print("Avg ");
    oled.print((float)avgCPM / 151.0);        // to μSv/h
    oled.println(unitStr);
    oled.print("Min ");
    oled.print((float)minCPM / 151.0);        // to μSv/h
    oled.println(unitStr);
    oled.print("Max ");
    oled.print((float)maxCPM / 151.0);        // to μSv/h
    oled.println(unitStr);
  }
  else
  {
    oled.setTextSize(1);                      // default 1:1 pixel scale
    oled.print("WiFi "); oled.print(WiFi.RSSI()); oled.println(" dBm");
    if (wifiStatus == WL_CONNECTED)
    {
      oled.print("IP   "); oled.println(WiFi.localIP());
      oled.print("GW   "); oled.println(WiFi.gatewayIP());
      oled.println();
      if (mqttClientConnected)
      {
        oled.println("MQTT connected");
        if (mqttEndMessageRet)
          oled.println("MQTT msg OK");
        else
          oled.println("MQTT msg ERROR");
      }
      else
      {
        oled.println("MQTT NOT connected:");
        oled.println(mqttClient.connectError());
      }
    }
    else
    {
      oled.println("WiFi NOT connected:");
      switch (wifiStatus)
      {
        case WL_IDLE_STATUS:          oled.println("WL_IDLE_STATUS"); break;
        case WL_NO_SSID_AVAIL:        oled.println("WL_NO_SSID_AVAIL"); break;
        case WL_CONNECT_FAILED:       oled.println("WL_CONNECT_FAILED"); break;
        case WL_CONNECTION_LOST:      oled.println("WL_CONNECTION_LOST"); break;
        case WL_DISCONNECTED:         oled.println("WL_DISCONNECTED"); break;
        case WL_NO_SHIELD:            oled.println("WL_NO_SHIELD"); break;
        default:                      oled.println(wifiStatus); break;
      }
    }
  }
  oled.display();
}
    
void loop()
{
  unsigned long currentMillis = millis();

  // Handle rotary encoder with button
  int encDirection = enc.read();
  if (encDirection == 1)
  {
    if (displayPage > 0)
      displayPage--;
    displayCurrentPage();
  }
  else if (encDirection == -1)
  {
    if (displayPage < DISPLAY_PAGES - 1)
      displayPage++;
    displayCurrentPage();
  }
  if (btn.pressed())
  {
    displayPage++;
    displayPage %= DISPLAY_PAGES;
    displayCurrentPage();
  }

  // Read detections count, send through MQTT and update OLED
  unsigned long diff = currentMillis - previousMillis;
  if (diff > SAMPLE_PERIOD_MS)
  {
    // Read and clear the count as fast a possible.
    // Note: the following disables interrupts; if an interrupt happens while the interrupts
    //       are disabled it is handled when the interrupts are re-enabled. The following 
    //       code is executed fast and more than one interrupt cannot happen while the 
    //       interrupts are disabled.
    unsigned long nowCount;
    ATOMIC()
    {
      nowCount = count;
      count = 0;
    }

    // Stats
    const unsigned long ONE_MINUTE_MS = 60000;
    nowCPM = nowCount * (ONE_MINUTE_MS / diff);
    if (nowCPM < minCPM) minCPM = nowCPM; 
    if (nowCPM > maxCPM) maxCPM = nowCPM;
    if (avgCPM == 0)
      avgCPM = nowCPM;
    else
    {
      uint64_t tmp = avgCPM;
      tmp *= 15;
      tmp += nowCPM;
      avgCPM = (unsigned long)(tmp / 16);
    }
    
    // WiFi status poll and (re-)connect
    wifiStatus = WiFi.status();
    if (wifiStatus != WL_CONNECTED)
    {
      WiFi.begin(ssid, pass); // some platforms have a blocking WiFi.begin(), others a non-blocking
      wifiStatus = WiFi.status();
    }
    
    // MQTT status poll and (re-)connect
    mqttClientConnected = 0;
    if (wifiStatus == WL_CONNECTED)
    {
      mqttClientConnected = mqttClient.connected();
      if (!mqttClientConnected)
        mqttClientConnected = mqttClient.connect(broker, port);
    }

    // MQTT publish
    mqttEndMessageRet = 0;
    if (mqttClientConnected)
    {
      mqttClient.beginMessage(topic);
      mqttClient.print("field1="); mqttClient.print((float)nowCPM / 151.0); // to μSv/h
      mqttEndMessageRet = mqttClient.endMessage();
    }

    // Update display
    displayCurrentPage();
    
    // Update previousMillis
    previousMillis = currentMillis;
  }

  // Send MQTT keep alive which avoids being disconnected by the broker
  mqttClient.poll();
}
