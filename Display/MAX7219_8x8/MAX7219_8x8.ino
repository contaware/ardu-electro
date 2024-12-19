/*
  MAX7219 8x8 led module
 
  - VDD supply is 5V, operating current with all LEDs ON at
    maximum brightness is 320mA.

  - The MAX7219 chip drives common-cathode displays. It can
    drive up to eight 7-segment displays or a 8x8 LED matrix.

  - The MAX7219 is driven by SPI, the LedControl library uses 
    the shiftOut() function for that.
  
  - We can daisy-chain multiple displays to make a larger display,
    connect the DOUT of the first display to the DIN of the next 
    display and so forth.
*/
#include "LedControl.h"         // by Eberhard Fahle
#define DEVICES_COUNT       4   // the number of chained devices

LedControl lc = LedControl( 12, // DIN 
                            10, // CLK
                            11, // LOAD(CS) 
                            DEVICES_COUNT);

// Create your own, use:
// https://jorydotcom.github.io/matrix-emoji/
byte digit[][8] = {
  {0x00, 0x3c, 0x24, 0x24, 0x24, 0x24, 0x24, 0x3c},  // 0
  {0x00, 0x04, 0x0c, 0x14, 0x24, 0x04, 0x04, 0x04},  // 1
  {0x00, 0x3c, 0x04, 0x04, 0x3c, 0x20, 0x20, 0x3c},  // 2
  {0x00, 0x3c, 0x04, 0x04, 0x1c, 0x04, 0x04, 0x3c},  // 3
  {0x00, 0x04, 0x0c, 0x14, 0x3e, 0x04, 0x04, 0x04},  // 4
  {0x00, 0x3c, 0x20, 0x20, 0x3c, 0x04, 0x04, 0x3c},  // 5
  {0x00, 0x3c, 0x20, 0x20, 0x3c, 0x24, 0x24, 0x3c},  // 6
  {0x00, 0x3c, 0x04, 0x04, 0x08, 0x10, 0x10, 0x10},  // 7
  {0x00, 0x3c, 0x24, 0x24, 0x3c, 0x24, 0x24, 0x3c},  // 8
  {0x00, 0x3c, 0x24, 0x24, 0x3c, 0x04, 0x04, 0x3c}   // 9
};

void setup()
{
  for (int addr = 0 ; addr < DEVICES_COUNT ; addr++)
  {
    // MAX7219 is in power-saving mode on startup, we have to do a wakeup call
    lc.shutdown(addr, false);
    
    // Set the brightness (0..15)
    lc.setIntensity(addr, 8);
  }
}

void demo(int addr)
{
  // Control single LEDs
  lc.clearDisplay(addr);
  for (int row = 0 ; row < 8 ; row++)
  {
    for (int col = 0 ; col < 8 ; col++)
    {
      lc.setLed(addr, row, col, true);
      delay(30);
    }
  }
  delay(1000);

  // Control rows
  lc.clearDisplay(addr);
  for (int row = 0 ; row < 8 ; row++)
  {
    lc.setRow(addr, row, 0b10101010);
    delay(100);
  }
  delay(1000);

  // Control columns
  lc.clearDisplay(addr);
  for (int col = 0 ; col < 8 ; col++)
  {
    lc.setColumn(addr, col, 0b10101010);
    delay(100);
  }
  delay(1000);

  // Count from 0..9
  for (int n = 0 ; n <= 9 ; n++)
  {
    for (int row = 0 ; row < 8 ; row++)
      lc.setRow(addr, row, digit[n][row]);
    delay(500);
  }
  delay(1000);
}

void loop()
{
  for (int addr = 0 ; addr < DEVICES_COUNT ; addr++)
    demo(addr);
}
