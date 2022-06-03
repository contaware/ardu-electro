/*
  Four 7seg display with TM1637 driver chip
  
  - VDD supply is 3.3V - 5V.
  - Segments:

        A
       ---
    F |   | B
       -G-
    E |   | C
       ---
        D
        
*/
#include <TM1637TinyDisplay.h> // by Jason Cox

const byte CLK_PIN = 2;
const byte DIO_PIN = 3;

// Create display object of type TM1637TinyDisplay
TM1637TinyDisplay display(CLK_PIN, DIO_PIN);

// Display array
uint8_t displayData[] = {0xff, 0xff, 0xff, 0xff};

// Set the individual segments per digit to spell words or create other symbols
const uint8_t done[] = {
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,           // d
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
  SEG_C | SEG_E | SEG_G,                           // n
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G            // E
};

// Create degree celsius symbol
const uint8_t celsius[] = {
  SEG_A | SEG_B | SEG_F | SEG_G,  // Circle
  SEG_A | SEG_D | SEG_E | SEG_F   // C
};

void setup()
{
  // Set brightness
  display.setBrightness(5);

  // Flip display upside down 
  // (useful when we mounted it the wrong way)
  display.flipDisplay(false);

  // Set the delay used to scroll a string with more than 4 chars
  display.setScrolldelay(200);  // library default is DEFAULT_SCROLL_DELAY which is 100 ms
  
  // Clear display
  display.clear();

  // Wait a moment
  delay(1000);
}

void loop()
{
  // Encode digits and show with a brightness from 0 (lowes brightness) 
  // to 7 (highest brightness).
  // Pass a number between 0 to 15; the numbers between 10-15 are 
  // converted to the hexadecimal digits: A, b, C, d, E, F
  displayData[0] = display.encodeDigit(0);
  displayData[1] = display.encodeDigit(1) | SEG_DP; // add dot / colon
  displayData[2] = display.encodeDigit(10);
  displayData[3] = display.encodeDigit(11);
  for (int k = 0; k < 8; k++)
  {
    display.setBrightness(k);
    display.setSegments(displayData);
    delay(500);
  }
  delay(1000);
  display.clear();
  delay(1000);
  
  // Show counter from -9 to 10
  for (int i = -9 ; i < 11 ; i++)
  {
    display.showNumber(i);
    delay(70);
  }
  delay(1000);
  display.clear();
  delay(1000);

  // Print floats
  display.showNumber(0.41);
  delay(2000);
  display.showNumber(-9.8);
  delay(2000);
  display.showNumber( -3.14,        // float or double number
                      2,            // show only this amount of digits after the decimal point
                      4,            // number of digits to update
                      0);           // position 0: leftmost, 3: rightmost
  delay(2000);
  display.clear();
  delay(1000);
  
  // Print numbers in different locations
  for (int i = 0; i < 4; i++)
  {
    display.showNumber(i,           // number
                       false,       // no leading zeros
                       1,           // number of digits to update
                       i);          // position 0: leftmost, 3: rightmost
    delay(500);
    display.clear();
  }
  delay(1000);
  display.clear();
  delay(1000);

  // Print 1234 with a dot or a colon (there exist different display types)
  // - For displays with dots between each digit:
  //   0.000   <-> 0b10000000
  //   00.00   <-> 0b01000000
  //   000.0   <-> 0b00100000
  //   0000.   <-> 0b00010000
  //   0.0.0.0 <-> 0b11100000
  // - For displays with just a colon:
  //   00:00   <-> 0b01000000
  // - For displays with dots and a colon:
  //   0.0:0.0 <-> 0b11100000
  display.showNumberDec(1234,       // number
                        0b01000000, // dots or colon
                        false,      // no leading zeros
                        4,          // number of digits to update
                        0);         // position 0: leftmost, 3: rightmost
  delay(2000);
  display.clear();
  delay(1000);

  // Level indicator
  for (int x = 0; x <= 100; x = x + 10)
  {
    display.showLevel(x,            // 0 .. 100
                      false);       // not horizontal
    delay(50);
  }
  for (int x = 100; x >= 0; x = x - 10)
  {
    display.showLevel(x,            // 100 .. 0
                      false);       // not horizontal
    delay(50);
  }
  delay(1000);
  display.clear();
  delay(1000);
  
  // Print temperature 1
  int temperature = 24;
  display.showNumber( temperature,  // number
                      false,        // no leading zeros
                      2,            // length
                      0);           // position
  display.setSegments(celsius,      // degree mark + C
                      2,            // length
                      2);           // position
  delay(2000);
  display.clear();
  delay(1000);

  // Print temperature 2
  display.showString( "\xB0",       // degree mark
                      1,            // length
                      3);           // position
  for (int x = -20; x < 110; x++)
  {
    display.showNumber( x,          // number
                        false,      // no leading zeros
                        3,          // length
                        0);         // position
    delay(10);
  }
  delay(1000);
  display.clear();
  delay(1000);

  // Say BLUE and HELLO
  display.showString("BLUE");
  delay(2000);
  display.clear();
  delay(1000);
  display.showString_P(PSTR("HELLO"));
  delay(1000);
  display.clear();
  delay(1000);
  
  // Manually say DONE
  display.setSegments(done);
  delay(2000);
  display.clear();
  delay(1000);
}
