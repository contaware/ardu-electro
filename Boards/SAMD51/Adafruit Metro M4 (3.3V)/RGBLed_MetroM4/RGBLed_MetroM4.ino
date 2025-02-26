/*
  Control the Adafruit Metro M4 RGB led with integrated WS2812B/SK6812 chip
*/
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel led(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

uint32_t currentColor = led.Color(0, 0, 255);

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
  led.begin();

  // Set the global brightness scaling (0..255, default: 255)
  // - Make your calcs according to your power supply current limit.
  // - Only set it once here in setup().
  led.setBrightness(10);

  // Show currentColor
  led.setPixelColor(0, currentColor);
  led.show();

  // Wait 3s
  delay(3000);
}
 
void loop()
{
  // Loop through RGB
  if (red(currentColor) == 255)
    currentColor = led.Color(0, 255, 0);
  else if (green(currentColor) == 255)
    currentColor = led.Color(0, 0, 255);
  else if (blue(currentColor) == 255)
    currentColor = led.Color(255, 0, 0);
  
  // Show currentColor
  led.setPixelColor(0, currentColor);
  led.show();

  // Wait 1s
  delay(1000);
}
