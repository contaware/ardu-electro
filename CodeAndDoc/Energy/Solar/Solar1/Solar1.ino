/*
  Portable solar project with Arduino Nano, oled display, touch button and sensors
  
  - The used Adafruit oled module has both a 3.3V LDO and level shifters so that it
    can be used together with other 5V I2C modules (connect Vin to 5V, there will
    be an unused 3.3V output on the 3Vo pin). Data means I2C SDA and Clk is I2C SCL.
    
  - The TTP223 touch button is already debounced.
*/
#include "TimerPoll.h"

// Touch button
const byte TOUCH_PIN = 13;
int previousTouchValue = -1;
unsigned long lastTouchPressMillis;
const unsigned long TOUCH_POLL_MS = 100;
TimerPoll timerTouch;

// Oled
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH            128         // OLED display width, in pixels, usually 128
#define SCREEN_HEIGHT           64          // OLED display height, in pixels, usually 64 or 32
#define SCREEN_ADDRESS          0x3D        // see board for Address: 0x3C or 0x3D
#define SSD1306_STARTUP_MS      500         // SSD1306 needs a small amount of time to be ready after initial power
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); // for STEMMA QT the RST pin is not necessary, so we pass -1
bool oledIsOn = false;
const unsigned long OLED_OFF_TIMEOUT_MS = 60000;
const unsigned long DISPLAY_RATE_MS = 1000;
TimerPoll timerDisplay;
uint64_t currentUpTime64;

// Battery voltage
const byte BATTERY_PIN = A3;

void setup()
{
  // Touch button
  pinMode(TOUCH_PIN, INPUT);
  timerTouch.begin(TOUCH_POLL_MS, touchPoll);
  lastTouchPressMillis = millis();

  // Oled
  timerDisplay.begin(DISPLAY_RATE_MS, doDisplay);
  currentUpTime64 = 0;
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
  oled.cp437(true);                         // use the 'Code Page 437'-compatible charset
  oled.setTextColor(SSD1306_WHITE);         // draw white text (must be called)
  oled.setCursor(0, 0);                     // start at top-left corner
}

void printUpTime(uint64_t upTime64)
{
  const uint64_t millisPerDay = 24ULL * 60 * 60 * 1000;
  int elapsedDays = (int)(upTime64 / millisPerDay);
  upTime64 = upTime64 % millisPerDay;
  
  const uint64_t millisPerHour = 60ULL * 60 * 1000;
  int elapsedHours = (int)(upTime64 / millisPerHour);
  upTime64 = upTime64 % millisPerHour;
  
  const uint64_t millisPerMin = 60ULL * 1000;
  int elapsedMin = (int)(upTime64 / millisPerMin);
  upTime64 = upTime64 % millisPerMin;
  
  const uint64_t millisPerSec = 1000ULL;
  int elapsedSec = (int)(upTime64 / millisPerSec);
  upTime64 = upTime64 % millisPerSec;

  char s[16];
  sprintf(s, "%d:%02d:%02d:%02d", elapsedDays, elapsedHours, elapsedMin, elapsedSec);
  oled.println(s);
}

void doDisplay()
{
  oled.clearDisplay();
  oled.setCursor(0, 0);
  
  // Display Up Time
  oled.setTextSize(2);                      // draw 2X-scale text
  oled.println("UpTime");
  oled.setTextSize(1);                      // restore default 1:1 pixel scale
  currentUpTime64 += DISPLAY_RATE_MS;       // doDisplay() calls are in sync with the millis() counter, no drift!
  printUpTime(currentUpTime64);

  // Display Battery voltage
  oled.setTextSize(2);                      // draw 2X-scale text
  oled.println("Battery");
  oled.setTextSize(1);                      // restore default 1:1 pixel scale
  float voltage = 5.0 * analogRead(BATTERY_PIN) / 1023.0;
  oled.print(voltage);
  oled.println("V");
  
  oled.display();
}

void touchPoll()
{
  int touchValue = digitalRead(TOUCH_PIN);
  unsigned long currentMillis = millis();

  // Display ON
  if (touchValue == 1)
  {
    lastTouchPressMillis = millis();
    if (!oledIsOn)
    {
      oled.ssd1306_command(SSD1306_DISPLAYON);
      oledIsOn = true;
    }
  }
  else
  {
    // Display OFF
    if (currentMillis - lastTouchPressMillis > OLED_OFF_TIMEOUT_MS)
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
    if (touchValue == 1)
    {
      // OFF -> ON
    }
    else
    { 
      // ON -> OFF
    }
    previousTouchValue = touchValue;
  }
}

void loop()
{
  timerTouch.process();
  timerDisplay.process();
}
