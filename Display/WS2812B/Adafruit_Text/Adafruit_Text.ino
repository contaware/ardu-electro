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
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN                 2
#define MATRIX_WIDTH            32
#define MATRIX_HEIGHT           8
#define TILESX                  1  // how many tiles in X direction
#define TILESY                  1  // how many tiles in Y direction
#define TEXT_TOP_OFFSET         0  // offset from top border
const char msg[] = "gulp!";
const int leftShiftOutChars = 6 * strlen(msg);

// Matrix layout flags:
// - Position of the FIRST LED in the matrix:
//   NEO_MATRIX_TOP, NEO_MATRIX_BOTTOM, NEO_MATRIX_LEFT, NEO_MATRIX_RIGHT
// - Proceed order of the LEDs:
//   NEO_MATRIX_ROWS or NEO_MATRIX_COLUMNS
// - Arrangement, where progressive means that the LEDs on all rows/columns 
//   progress in the same direction, while zigzag reverses the direction with 
//   each new row/column:
//   NEO_MATRIX_PROGRESSIVE or NEO_MATRIX_ZIGZAG
//
// Tiles layout flags:
// - Position of the FIRST tile in the overall arrangement:
//   NEO_TILE_TOP, NEO_TILE_BOTTOM, NEO_TILE_LEFT, NEO_TILE_RIGHT
// - Proceed order of the tiles:
//   NEO_TILE_ROWS or NEO_TILE_COLUMNS
// - Arrangement, where progressive means that the tiles on all rows/columns 
//   progress in the same direction, while zigzag reverses the direction with 
//   each new row/column (also the single tiles must be rotated by 180°):
//   NEO_TILE_PROGRESSIVE or NEO_TILE_ZIGZAG
//
// Pixel type flags:
// - NEO_KHZ800: 800 KHz bitstream (most products)
// - NEO_KHZ400: 400 KHz (WS2811)
// - NEO_GRB:    Pixels are wired for GRB bitstream (most products)
// - NEO_RGB:    Pixels are wired for RGB bitstream (v1 FLORA pixels)
//
// After this configuration, the rest of the sketch never needs to think 
// about the layout. Coordinate (0,0) for drawing graphics will always 
// be at the top-left of the overall arrangement.
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(
  MATRIX_WIDTH,
  MATRIX_HEIGHT,
  TILESX,
  TILESY,
  LED_PIN,
  NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG +
  NEO_TILE_TOP + NEO_TILE_LEFT + NEO_TILE_ROWS + NEO_TILE_PROGRESSIVE,
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

int x = matrix.width();     // that's the overall width
unsigned int pass = 0;

void loop()
{
  matrix.fillScreen(0);     // turn off all (pixel in 16-bit '565' RGB format)
  matrix.setCursor(x, TEXT_TOP_OFFSET);
  matrix.print(msg);

  // Reset x position?
  if (--x < -leftShiftOutChars)
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
