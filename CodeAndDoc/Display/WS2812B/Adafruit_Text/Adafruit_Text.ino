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

  - The library supports also tiling matrices to create a big display.

  - Terminology: NeoPixel is Adafruit's brand for individually-addressable 
                 RGB color pixels and strips based on WS2812B/SK6812.
*/
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN                 2
#define MATRIX_WIDTH            32
#define MATRIX_HEIGHT           8

// Layout flags:
// - Position of the FIRST LED in the matrix:
//   NEO_MATRIX_TOP, NEO_MATRIX_BOTTOM, NEO_MATRIX_LEFT, NEO_MATRIX_RIGHT
// - Proceed order:
//   NEO_MATRIX_ROWS or NEO_MATRIX_COLUMNS
// - Arrangement:
//   NEO_MATRIX_PROGRESSIVE or NEO_MATRIX_ZIGZAG
// Pixel type flags:
// - NEO_KHZ800: 800 KHz bitstream (most products)
// - NEO_KHZ400: 400 KHz (WS2811)
// - NEO_GRB:    Pixels are wired for GRB bitstream (most products)
// - NEO_RGB:    Pixels are wired for RGB bitstream (v1 FLORA pixels)
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(
  MATRIX_WIDTH,
  MATRIX_HEIGHT,
  LED_PIN,
  NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB + NEO_KHZ800);

const uint16_t colors[] =
{
  matrix.Color(255, 0, 0), 
  matrix.Color(0, 255, 0), 
  matrix.Color(0, 0, 255),
  matrix.Color(255, 255, 255)
};

void setup()
{
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setTextSize(1);    // 1 is the default 6x8, 2 is 12x16, 3 is 18x24, etc
  matrix.setRotation(0);    // 0 thru 3 corresponding to 4 cardinal rotations
  matrix.setBrightness(10); // 0..255. Only set it once here in setup()
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
