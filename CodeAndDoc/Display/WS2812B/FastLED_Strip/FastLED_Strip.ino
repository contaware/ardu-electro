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

  - Terminology: NeoPixels is Adafruit's brand for individually-addressable 
                 RGB leds based on WS2812B/SK6812.
*/
#include <FastLED.h> // by Daniel Garcia

#define LED_PIN     2
#define NUM_LEDS    256

CRGB currentColor = CRGB(0, 0, 255);
CRGB leds[NUM_LEDS];
 
void setup()
{
  // Init
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS); // color order: RGB, RBG, GRB, GBR, BRG, BGR

  // Set the global brightness scaling (0..255, default: 255)
  FastLED.setBrightness(10);

  // Set the maximum power to be used (that limits the brightness)
  // Note: the Arduino 5V output pin is good for ~400 mA on USB and
  //       ~900 mA when using an external power adapter.
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 400); // led volts (usually 5), max milliamps you can supply
  
  // Wipe the local array of data without updating the LEDs
  FastLED.clear();

  /*
   Different ways to set a led color:
   - leds[i].red = 255;
   - leds[i].r = 255;
   - leds[i] = CRGB::Red; // named HTML web color
   - leds[i] = CRGB(255, 0, 0);
   - leds[i].setRGB(255, 0, 0);
   - leds[i] = 0xFF0000;  // 0xRRGGBB
   - leds[i] = leds[j];
  */
  leds[0] = CRGB(255, 0, 0);
  leds[1] = CRGB(0, 255, 0);
  leds[2] = CRGB(0, 0, 255);
  
  // Update all our controllers with the current led colors
  FastLED.show();

  // Wait a moment
  delay(3000);
}
 
void loop()
{
  if (currentColor.r == 255)
    currentColor = CRGB(0, 255, 0);
  else if (currentColor.g == 255)
    currentColor = CRGB(0, 0, 255);
  else if (currentColor.b == 255)
    currentColor = CRGB(255, 0, 0);
  
  for (int i = 0 ; i < NUM_LEDS ; i++)
  {
    if (i == 0)
      leds[NUM_LEDS - 1] = CRGB(0, 0, 0);
    else
      leds[i - 1] = CRGB(0, 0, 0);
    leds[i] = currentColor;
    FastLED.show();
    delay(80);
  }
}
