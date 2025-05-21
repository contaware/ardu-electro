/*
  Freenove ESP32 board's WS2812B led

  - The board has a single WS2812B led.

  - Note that this Freenove library supports also strips with 
    multiple WS2812B leds.
*/
#include "Freenove_WS2812_Lib_for_ESP32.h" // Freenove WS2812 Lib for ESP32 by Freenove

#define WS2812B_PIN       16
#define RMT_CHANNEL       0   // RMT module channel
Freenove_ESP32_WS2812 strip = Freenove_ESP32_WS2812(1, WS2812B_PIN, RMT_CHANNEL, TYPE_GRB);

void setup()
{
  strip.begin();
  strip.setBrightness(10); // 0..255
}

void loop()
{
  // Red
  strip.setLedColorData(0, 255, 0, 0);
  strip.show();
  delay(1000);

  // Green
  strip.setLedColorData(0, 0, 255, 0);
  strip.show();
  delay(1000);
  
  // Blue
  strip.setLedColorData(0, 0, 0, 255);
  strip.show();
  delay(1000);
}
