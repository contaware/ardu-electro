/*
  5050 RGB led strip with integrated WS2812B chips 
  (SK6812 is a clone of WS2812B)

  - FastLED compared to Adafruit::Neopixel is considered faster and uses 
    static memory allocation for the LED data. Adafruit::Neopixel consumes 
    less program storage space and uses malloc() for the LED data.

  - VDD supply is 5V. You need level-shifters if you want to control 
    these LEDs with 3.3V levels.
  
  - Each LED uses up to 60mA, so make your calcs!
  
  - Add a 220Ω - 470Ω resistor between the Arduino digital output pin 
    and the data input pin to reduce noise on that line.

  - The control chip receives a serial input, subtracts its own 24-bits,
    and passes the remaining data to the next chip in the chain. Each
    pixel is sent as GRB, with the most significant bit first:
    G7 G6 G5 G4 G3 G2 G1 G0 R7 R6 R5 R4 R3 R2 R1 R0 B7 B6 B5 B4 B3 B2 B1 B0
                       ___                                 ______
  - A 0 is sent like: |   |______|      A 1 is sent like: |      |___|
                       T0H   T0L                            T1H   T1L
                     0.35µs   0.9µs                       0.9µs   0.35µs
    Note: the duration of a bit is always 1.25µs.

  - To restart a cycle send a reset: |__________________|
                                            >50µs
    Note: since V4 of the spec, a reset must be >280µs instead of >50µs.

  - Terminology: NeoPixel is Adafruit's brand for individually-addressable 
                 RGB color pixels and strips based on WS2812B/SK6812.
*/
#include <Adafruit_NeoPixel.h>

#define LED_PIN     2
#define NUM_LEDS    256

// Pixel type flags:
// - NEO_KHZ800: 800 KHz bitstream (most products)
// - NEO_KHZ400: 400 KHz (WS2811)
// - NEO_GRB:    Pixels are wired for GRB bitstream (most products)
// - NEO_RGB:    Pixels are wired for RGB bitstream (v1 FLORA pixels)
// - NEO_RGBW:   Pixels are wired for RGBW bitstream (RGBW products)
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

uint32_t currentColor = strip.Color(0, 0, 255);

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
  strip.begin();

  // Set the global brightness scaling (0..255, default: 255)
  // - Make your calcs according to your power supply current limit.
  // - Only set it once here in setup().
  strip.setBrightness(10);
  
  // Set all to 0
  strip.clear();

  // Set led colors
  strip.setPixelColor(0, 255, 0, 0);
  strip.setPixelColor(1, 0, 255, 0);
  strip.setPixelColor(2, 0, 0, 255);
  
  // Update all our controllers with the current led colors
  strip.show();

  // Wait a moment
  delay(3000);
}
 
void loop()
{
  if (red(currentColor) == 255)
    currentColor = strip.Color(0, 255, 0);
  else if (green(currentColor) == 255)
    currentColor = strip.Color(0, 0, 255);
  else if (blue(currentColor) == 255)
    currentColor = strip.Color(255, 0, 0);
  
  for (uint16_t i = 0 ; i < strip.numPixels() ; i++)
  {
    if (i == 0)
      strip.setPixelColor(strip.numPixels() - 1, 0, 0, 0);
    else
      strip.setPixelColor(i - 1, 0, 0, 0);
    strip.setPixelColor(i, currentColor);
    strip.show();
    delay(80);
  }
}
