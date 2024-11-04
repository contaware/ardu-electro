/*
  MAX7219 8x8 led module
 
  VDD supply is 5V.
*/
#include "LedControl.h"

LedControl lc=LedControl( 12, // DataIn 
                          10, // CLK
                          11, // LOAD(CS) 
                          1); // we have only a single MAX7219

const unsigned long delaytime = 500;

void setup()
{
  // MAX7219 is in power-saving mode on startup, we have to do a wakeup call
  lc.shutdown(0, false);
  
  // Set the brightness to a medium values
  lc.setIntensity(0, 8);
  
  // Clear the display
  lc.clearDisplay(0);
}

void loop()
{ 
  lc.setLed(0, 0, 0, true); delay(delaytime); lc.setLed(0, 0, 0, false);
  lc.setLed(0, 0, 1, true); delay(delaytime); lc.setLed(0, 0, 1, false);
  lc.setRow(0, 2 , 0b10001000); delay(delaytime); lc.setRow(0, 2, 0); // B10001000 is the Arduino notation and works only for bytes, C notation is 0b10001000 or 0B10001000 and works for all sizes
}
