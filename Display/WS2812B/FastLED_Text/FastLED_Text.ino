/*
  5050 RGB led matrix with integrated WS2812B chips 
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
    
  - The library supports also tiling matrices to create a big display.

  - Terminology: NeoPixels is Adafruit's brand for individually-addressable 
                 RGB leds based on WS2812B/SK6812.
*/
#include <Adafruit_GFX.h>
#include <Framebuffer_GFX.h>    // by Marc Merlin
#include <FastLED_NeoMatrix.h>  // by Marc Merlin
#include <FastLED.h>            // by Daniel Garcia

#define LED_PIN                 2
#define MATRIX_WIDTH            32
#define MATRIX_HEIGHT           8
#define NUM_LEDS                MATRIX_WIDTH*MATRIX_HEIGHT
CRGB leds[NUM_LEDS];

// Layout flags:
// - Position of the FIRST LED in the matrix:
//   NEO_MATRIX_TOP, NEO_MATRIX_BOTTOM, NEO_MATRIX_LEFT, NEO_MATRIX_RIGHT
// - Proceed order:
//   NEO_MATRIX_ROWS or NEO_MATRIX_COLUMNS
// - Arrangement:
//   NEO_MATRIX_PROGRESSIVE or NEO_MATRIX_ZIGZAG
FastLED_NeoMatrix matrix = FastLED_NeoMatrix(
  leds,
  MATRIX_WIDTH,
  MATRIX_HEIGHT,
  NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG);

const uint16_t colors[] =
{
  matrix.Color(255, 0, 0), 
  matrix.Color(0, 255, 0), 
  matrix.Color(0, 0, 255),
  matrix.Color(255, 255, 255)
};

void setup()
{
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS); // color order: RGB, RBG, GRB, GBR, BRG, BGR
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setTextSize(1);    // 1 is the default 6x8, 2 is 12x16, 3 is 18x24, etc
  matrix.setRotation(0);    // 0 thru 3 corresponding to 4 cardinal rotations
  matrix.setBrightness(10); // 0..255
  matrix.setTextColor(colors[0]);
}

int x = matrix.width();
unsigned int pass = 0;

void loop()
{
  matrix.fillScreen(0); // turn off all (pixel in 16-bit '565' RGB format)
  matrix.setCursor(x, 0);
  matrix.print("gulp!");

  // Reset x position?
  if (--x < -matrix.width())
  {
    // Reset x position
    x = matrix.width();

    // Change color
    if (++pass >= sizeof(colors) / sizeof(colors[0]))
      pass = 0;
    matrix.setTextColor(colors[pass]);
  }

  // Display
  matrix.show();

  // For a bigger matrix the refresh rate from show() will
  // be slow enough that a delay is not necessary.
  delay(30);
}
