/*
  UNO R4 WiFi built-in 12x8(=96) LED Matrix

  - Do not use on()/off() to turn ON/OFF individual LEDS, it's not 
    working like that, control the LEDs through an array of 96-bytes or 
    more efficiently with an array of 96-bits (3 x 32-bit words).

  - There is a LED Matrix editor to create the arrays:
    https://ledmatrix-editor.arduino.cc
*/
#include "Arduino_LED_Matrix.h"
ArduinoLEDMatrix matrix;

// Array of 96-bytes
byte frame_bytes[8][12] = {
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }
};

// Array of 96-bits (3 x 32-bit words)
// Led location is associated to the bits like you write them out
// and the rows are divided in 12-bits groups.
void SetLEDBit(unsigned long* bitsArray, int idx, int value)
{
  int word32Num = idx / 32;
  int bitNum = idx % 32;
  bitWrite(bitsArray[word32Num], 31 - bitNum, value); // do 31 - bitNum because this function counts starting from the
                                                      // least significant bit of the 32-bit bitsArray[word32Num]
}
unsigned long frame_bits[] = {
              //   [   row0   ][   row1   ][   row2
  0x3184a444, // 0b00110001100001001010010001000100
  
              //  row2][   row3   ][   row4   ][row5
  0x42081100, // 0b01000010000010000001000100000000
  
              //   row5   ][   row6   ][   row7   ]
  0xa0040000  // 0b10100000000001000000000000000000
};

// Animation sequence with the frames described
// by 3 x 32-bit words and a show time
const unsigned long anim_frames[][4] = {
  {
    0,
    0,
    0,
    200 // delay in ms
  },
  { // Happy
    0x00019819,
    0x80000001,
    0x081f8000,
    800 // delay in ms
  },
  {
    0,
    0,
    0,
    200 // delay in ms
  },
  { // Danger
    0x0400a015,
    0x01502082,
    0x484047fc,
    800 // delay in ms
  }
};

void setup()
{
  // Define LED_BUILTIN as output
  pinMode(LED_BUILTIN, OUTPUT);

  // Init
  matrix.begin();

  /*
    Bytes array
    Note that renderBitmap() is defined as:
    #define renderBitmap(bitmap, rows, columns) loadPixels(&bitmap[0][0], rows*columns)
  */
  matrix.renderBitmap(frame_bytes, 8, 12); // rows x columns
  delay(1500);
  frame_bytes[7][11] = 0; // turn bottom-right LED OFF for a moment and then ON again
  matrix.renderBitmap(frame_bytes, 8, 12);
  delay(1000);
  frame_bytes[7][11] = 1;
  matrix.renderBitmap(frame_bytes, 8, 12);
  delay(1500);

  /*
    More efficient bits array
  */
  matrix.loadFrame(frame_bits);
  for (int i = 0 ; i < 96 ; i++)    // all OFF
    SetLEDBit(frame_bits, i, false);
  matrix.loadFrame(frame_bits);
  for (int i = 0 ; i < 96 ; i++)
  {
    SetLEDBit(frame_bits, i, true); // one ON a the time
    matrix.loadFrame(frame_bits);
    delay(20);
  }

  /*
    Sequence play
    Note that loadSequence() is defined as:
    #define loadSequence(frames)                loadWrapper(frames, sizeof(frames))
  */
  matrix.loadSequence(anim_frames);
  matrix.play(true);  // true=play the animation infinitely,
                      // false=play one time and stop at last frame
}

void loop()
{
  // Blinks the built-in LED while the animation is playing
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}
