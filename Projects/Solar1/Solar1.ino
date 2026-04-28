/*
  Portable solar project with Arduino Nano, oled display, touch button 
  and sensors
  
  - The used Adafruit oled module has both a 3.3V LDO and level shifters 
    so that it can be used together with other 5V I2C modules (connect 
    Vin to 5V, there will be an unused 3.3V output on the 3Vo pin). 
    Data means I2C SDA and Clk is I2C SCL.
    
  - The TTP223 touch button is already debounced.

  - DHT22 sampling rate is 0.5 Hz -> poll slower than that, otherwise an 
    error is returned.

  - The Guva-S12SD sensor covers all UVB and most of the UVA spectrum 
    (240nm - 370nm).
    
  - A solar panel with a MPP voltage greater than 6V is connected 
    through a Buck converter (XL4015 with MPP regulation) to the 
    charger (MCP73871). The charger output comes either from solar or 
    battery, and thus it varies between 3V and 6V. The NANO and all 
    modules are powered by the 5V from the Buck-Boost converter 
    (TPS63060) connected to the charger output.

  - To optimize consumption:
    1. Desolder all power LEDs from all modules. 
    2. Desolder Nano's 5V 1117 LDO regulator -> bridge tab with pin 2. 
    3. Desolder Nano's FT232/CH340 chip -> must use ISP to program Nano. 
    The Nano and the attached modules ended up to use less than 1mA when 
    calling LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF). The 
    problem remains the Buck-Boost converter (TPS63060) which uses 13mA 
    @ 3.7V even if it only needs to provide less than 1mA @ 5V. 
*/
#include "TimerPoll_CW.h"
#include "PrintCol_CW.h"
#include <SimpleDHT.h> // by Winlin
#include "LowPower.h"  // Low-Power by Rocket Scream Electronics

// Touch button
const byte TOUCH_PIN = 2;
volatile byte touchValue = 0;
unsigned long lastTouchMillis;
const unsigned long TOUCH_POLL_MS = 100;
TimerPoll_CW timerTouch;

// Measure the charger input voltage with a divide by two 
// voltage divider (2x 18kΩ resistors)
// Note: the MCP73871 charger supports a maximum of 6V at its input
const byte CHARGER_PIN = A2;

// Measure the battery voltage using a 10kΩ resistor to 
// avoid powering the Arduino through this pin!
const byte BATTERY_PIN = A3;

// Sensors
const byte UV_PIN = A0;
const byte DHT_PIN = 8;
float voltCharger = 0.0;
float voltBattery = 0.0;
float uvValue = 0.0;
SimpleDHT22 dht22(DHT_PIN);
float temperature = 0.0;
float humidity = 0.0;
const unsigned long SENSORS_POLL_MS = 2543;
TimerPoll_CW timerSensors;

// Oled
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH            128             // OLED display width, in pixels, usually 128
#define SCREEN_HEIGHT           64              // OLED display height, in pixels, usually 64 or 32
#define SCREEN_ADDRESS          0x3D            // see board for Address: 0x3C or 0x3D
#define SSD1306_STARTUP_MS      500             // SSD1306 needs a small amount of time to be ready after initial power
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); // for STEMMA QT the RST pin is not necessary, so we pass -1
int displayPage = 0;
const int DISPLAY_PAGES = 2;
bool oledIsOn = false;
const unsigned long OLED_OFF_TIMEOUT_MS = 60000;

void touchPress()
{
  touchValue = 1;
}

void setup()
{
  // Touch button
  pinMode(TOUCH_PIN, INPUT);
  timerTouch.begin(TOUCH_POLL_MS, touchPoll);
  lastTouchMillis = millis();
  attachInterrupt(digitalPinToInterrupt(TOUCH_PIN), touchPress, RISING);
  
  // Sensors
  timerSensors.begin(SENSORS_POLL_MS, sensorsPoll);
  
  // Oled
  /*
    When powering the device if your code tries to write to the display too soon,
    it just shows a black screen (oled.begin() succeeds but nothing works).
    Note that it will work on reset since that typically does not cycle power. 
    See: https://learn.adafruit.com/monochrome-oled-breakouts/troubleshooting-2
  */
  delay(SSD1306_STARTUP_MS);
  oled.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS); // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  oled.clearDisplay();
  oled.display();
  oled.ssd1306_command(SSD1306_DISPLAYON);
  oledIsOn = true;
  oled.cp437(true);                             // use the 'Code Page 437'-compatible charset
  oled.setTextColor(SSD1306_WHITE);             // draw white text (must be called)
  oled.setTextSize(2);                          // draw 2X-scale text
}

void displayPage0()
{
  oled.clearDisplay();
  oled.setCursor(0, 0);

  // Charger input voltage
  oled.print("Chr");
  printCol(voltCharger, 2, 6, oled);
  oled.println("V");
  
  // Battery voltage
  oled.print("Bat");
  printCol(voltBattery, 2, 6, oled);
  oled.println("V");

  // Temperature
  oled.print("T");
  printCol(temperature, 1, 7, oled);
  oled.write(0xF8); // cp437 code for °
  oled.println("C");

  // Relative humidity
  oled.print("Rh");
  printCol(humidity, 1, 7, oled);
  oled.println("%");
  
  oled.display();
}

void displayPage1()
{
  oled.clearDisplay();
  oled.setCursor(0, 0);

  oled.print("Uv");
  printCol(uvValue, 1, 8, oled);
  oled.println();
  oled.setTextSize(1);
  oled.println();
  oled.println("0..2    low");
  oled.println("3..5    medium");
  oled.println("6..7    high");
  oled.println("8..10   very high");
  oled.println(">=11    stay inside!");
  oled.setTextSize(2);
  
  oled.display();
}

void displayCurrentPage()
{
  switch (displayPage)
  {
    case 0:   displayPage0(); break;
    case 1:   displayPage1(); break;
    default:  break;
  }
}

void touchPoll()
{
  unsigned long currentMillis = millis();

  if (touchValue == 1)
  {
    touchValue = 0;
    lastTouchMillis = currentMillis;
    if (oledIsOn)
    {
      displayPage++;
      displayPage %= DISPLAY_PAGES;
    }
    else
    {
      oled.ssd1306_command(SSD1306_DISPLAYON);
      oledIsOn = true;
    }
    displayCurrentPage();
  }
  else
  {
    if (currentMillis - lastTouchMillis > OLED_OFF_TIMEOUT_MS && oledIsOn)
    {
      oled.ssd1306_command(SSD1306_DISPLAYOFF);
      oledIsOn = false;
      LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
    }
  }
}

void sensorsPoll()
{
  voltCharger = 10.0 * analogRead(CHARGER_PIN) / 1023.0;
  voltBattery = 5.0 * analogRead(BATTERY_PIN) / 1023.0;
  uvValue = 50.0 * analogRead(UV_PIN) / 1023.0;
  dht22.read2(&temperature, &humidity, NULL);
  displayCurrentPage();
}

void loop()
{
  timerTouch.process();
  timerSensors.process();
}
