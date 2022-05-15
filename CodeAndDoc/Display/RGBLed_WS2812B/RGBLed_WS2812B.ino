/*
  5050 RGB led strip or matrix with integrated WS2812B chips (SK6812 is a clone of WS2812B)
 
  - VDD supply is 5V.
  - Each LED uses up to 60mA so make your calcs.
  - Add a 220 - 470 Ohm resistor between the Arduino digital output pin and the
    data input pin to reduce noise on that line.
*/
#include <FastLED.h>

#define LED_PIN     2
#define NUM_LEDS    8*8

CRGB currentColor = CRGB(0, 0, 255);
CRGB leds[NUM_LEDS];
 
void setup()
{
  Serial.begin(9600);
  
  // Init
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS); // color order: RGB, RBG, GRB, GBR, BRG, BGR
  
  // Set the global brightness scaling (default: 255)
  // 0..255 value for how much to scale all leds before writing them out
  FastLED.setBrightness(100);
  
  // Get the current global brightness setting
  Serial.print("Current global Brightness: ");
  Serial.println(FastLED.getBrightness());
  
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
