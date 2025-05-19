/*
  MAX7219 7seg module
 
  - VDD supply is 5V, chip outputs a maximum of 330mA.
    Note: the chip specifies a minimum logic high input of 3.5V. It 
          may work with your 3.3V Arduino, but this is not guaranteed.

  - The chip drives common-cathode displays. It can drive up to eight 
    7-segment displays or a 8x8 LED matrix.

  - The LedControl library uses the shiftOut() function to drive the 
    chip.
  
  - We can daisy-chain multiple displays to make a larger display, 
    connect the DOUT of the first display to the DIN of the next 
    display and so forth.
*/
#include "LedControl.h"         // by Eberhard Fahle
#define DEVICES_COUNT       1   // the number of chained devices

LedControl lc = LedControl( 12, // DIN 
                            10, // CLK
                            11, // LOAD(CS) 
                            DEVICES_COUNT);

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
  // Numbers count-up on first digit 
  lc.clearDisplay(addr);
  int digit = 0;
  for (byte value = 0 ; value <= 0xF ; value++)
  {
    lc.setDigit(addr, digit, value, false); // true to set DP
    delay(500);
  }
  delay(1000);

  // Display supported chars on second digit
  const char chars7seg[] = {'0','1','2','3','4','5','6','7','8','9',
                            'A','b','c','d','E','F','H','L','P',
                            '.','-','_',' '};
  lc.clearDisplay(addr);
  digit = 1;
  for (size_t i = 0 ; i < sizeof(chars7seg) ; i++)
  {
    lc.setChar(addr, digit, chars7seg[i], false); // true to set DP
    delay(500);
  }
  delay(1000);
}

void loop()
{
  for (int addr = 0 ; addr < DEVICES_COUNT ; addr++)
    demo(addr);
}
