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
*/
#include "TimerPoll.h"
#include "PrintCol.h"
#include <SimpleDHT.h> // by Winlin

// Touch button
const byte TOUCH_PIN = 13;
int previousTouchValue = -1;
unsigned long lastTouchMillis;
const unsigned long TOUCH_POLL_MS = 100;
TimerPoll timerTouch;

// Charger input voltage (using a 1/2 voltage divider)
// Note: the charger supports a maximum of 6V at its input
const byte CHARGER_PIN = A2;

// Battery voltage
const byte BATTERY_PIN = A3;

// Uv
const byte UV_PIN = A0;

// DHT22 humidity and temperature sensor
const byte DHT_PIN = 8;
SimpleDHT22 dht22(DHT_PIN);
float temperature = 0.0;
float humidity = 0.0;
const unsigned long TEMP_HUMID_POLL_MS = 2543;
TimerPoll timerTempHumid;

// UpTime
uint64_t currentUpTime64 = 0;
const unsigned long SEC_TICK_MS = 1000;
TimerPoll timerSecTick;

// Oled
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH            128             // OLED display width, in pixels, usually 128
#define SCREEN_HEIGHT           64              // OLED display height, in pixels, usually 64 or 32
#define SCREEN_ADDRESS          0x3D            // see board for Address: 0x3C or 0x3D
#define SSD1306_STARTUP_MS      500             // SSD1306 needs a small amount of time to be ready after initial power
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); // for STEMMA QT the RST pin is not necessary, so we pass -1
int displayPage = 0;
const int DISPLAY_PAGES = 3;
bool oledIsOn = false;
const unsigned long OLED_OFF_TIMEOUT_MS = 60000;

void setup()
{
  // Touch button
  pinMode(TOUCH_PIN, INPUT);
  timerTouch.begin(TOUCH_POLL_MS, touchPoll);
  lastTouchMillis = millis();

  // UpTime
  timerSecTick.begin(SEC_TICK_MS, onSecTick);
  
  // Temperature and humidity
  timerTempHumid.begin(TEMP_HUMID_POLL_MS, tempHumidPoll);
  
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
  float voltage = 10.0 * analogRead(CHARGER_PIN) / 1023.0;
  oled.print("Chr");
  printCol(voltage, 2, 6, oled);
  oled.println("V");
  
  // Battery voltage
  voltage = 5.0 * analogRead(BATTERY_PIN) / 1023.0;
  oled.print("Bat");
  printCol(voltage, 2, 6, oled);
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
  printCol(50.0 * analogRead(UV_PIN) / 1023.0, 1, 8, oled);
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

void displayPage2()
{
  oled.clearDisplay();
  oled.setCursor(0, 0);

  uint64_t upTime64 = currentUpTime64; 
  const uint64_t millisPerDay = 24ULL * 60 * 60 * 1000;
  int elapsedDays = (int)(upTime64 / millisPerDay);
  upTime64 %= millisPerDay;
  
  const uint64_t millisPerHour = 60ULL * 60 * 1000;
  int elapsedHours = (int)(upTime64 / millisPerHour);
  upTime64 %= millisPerHour;
  
  const uint64_t millisPerMin = 60ULL * 1000;
  int elapsedMin = (int)(upTime64 / millisPerMin);
  upTime64 %= millisPerMin;
  
  const uint64_t millisPerSec = 1000ULL;
  int elapsedSec = (int)(upTime64 / millisPerSec);
  upTime64 %= millisPerSec;

  oled.print("Days");
  printCol(elapsedDays, DEC, 6, oled);
  oled.println();

  oled.print("Hours");
  printCol(elapsedHours, DEC, 5, oled);
  oled.println();
  
  oled.print("Min");
  printCol(elapsedMin, DEC, 7, oled);
  oled.println();
  
  oled.print("Sec");
  printCol(elapsedSec, DEC, 7, oled);
  oled.println();
  
  oled.display();
}

void displayCurrentPage()
{
  switch (displayPage)
  {
    case 0:   displayPage0(); break;
    case 1:   displayPage1(); break;
    default:  displayPage2(); break;
  }
}

void touchPoll()
{
  bool hasTurnedOn = false;
  int touchValue = digitalRead(TOUCH_PIN);
  unsigned long currentMillis = millis();

  // Turn display ON?
  if (touchValue == 1)
  {
    lastTouchMillis = currentMillis;
    if (!oledIsOn)
    {
      oled.ssd1306_command(SSD1306_DISPLAYON);
      oledIsOn = true;
      hasTurnedOn = true;
      displayCurrentPage();
    }
  }
  else
  {
    // Turn display OFF?
    if (currentMillis - lastTouchMillis > OLED_OFF_TIMEOUT_MS)
    {
      if (oledIsOn)
      {
        oled.ssd1306_command(SSD1306_DISPLAYOFF);
        oledIsOn = false;
      }
    }
  }

  // Handle touch change
  if (previousTouchValue != touchValue)
  {
    if (touchValue == 1 && !hasTurnedOn)
    {
      displayPage++;
      displayPage %= DISPLAY_PAGES;
      displayCurrentPage();
    }
    previousTouchValue = touchValue;
  }
}

void onSecTick()
{
  currentUpTime64 += SEC_TICK_MS; // onSecTick() calls are in sync with the millis() counter, no drift!
  if (oledIsOn)
    displayCurrentPage();
}

void tempHumidPoll()
{
  dht22.read2(&temperature, &humidity, NULL);
}

void loop()
{
  timerTouch.process();
  timerSecTick.process();
  timerTempHumid.process();
}
