/*
  Control the 3X RGB leds with integrated WS2812B/SK6812 chip
*/
#include <Adafruit_NeoPixel.h>

#define LED_PIN     2
#define NUM_LEDS    3

Adafruit_NeoPixel leds(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

uint32_t currentColor = leds.Color(0, 0, 255);

uint8_t red(uint32_t color)
{
  return (color >> 16) & 0xFF;
}

uint8_t green(uint32_t color)
{
  return (color >> 8) & 0xFF;
}

uint8_t blue(uint32_t color)
{
  return color & 0xFF;
}

void setup()
{
  // Init
  leds.begin();

  // Set the global brightness scaling (0..255, default: 255)
  // - Make your calcs according to your power supply current limit.
  // - Only set it once here in setup().
  leds.setBrightness(10);
  
  // Set all to 0
  leds.clear();

  // Set led colors
  leds.setPixelColor(0, 255, 0, 0);
  leds.setPixelColor(1, 0, 255, 0);
  leds.setPixelColor(2, 0, 0, 255);
  
  // Update all our controllers with the current led colors
  leds.show();

  // Wait a moment and then clear-all
  delay(3000);
  leds.clear();
  leds.show();
}
 
void loop()
{
  if (red(currentColor) == 255)
    currentColor = leds.Color(0, 255, 0);
  else if (green(currentColor) == 255)
    currentColor = leds.Color(0, 0, 255);
  else if (blue(currentColor) == 255)
    currentColor = leds.Color(255, 0, 0);
  
  for (uint16_t i = 0 ; i < leds.numPixels() ; i++)
  {
    if (i == 0)
      leds.setPixelColor(leds.numPixels() - 1, 0, 0, 0);
    else
      leds.setPixelColor(i - 1, 0, 0, 0);
    leds.setPixelColor(i, currentColor);
    leds.show();
    delay(1000);
  }
}
