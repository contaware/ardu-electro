/*
  MCP4725 12-Bit DAC module
 
  VDD supply is 2.7V - 5.5V.
*/
#include <Adafruit_MCP4725.h>

Adafruit_MCP4725 dac;

void setup()
{
  // For Adafruit MCP4725 the address is 0x62 (or 0x63 if A0 pin tied to VCC)
  dac.begin(0x62);
}

void loop()
{
    uint32_t counter;
    
    // Run through the full 12-bit scale for a triangle wave
    // Note: that will generate a wave with a frequency less than 1Hz 
    //       because I2C is quite slow
    for (counter = 0; counter < 4095; counter++)
    {
      dac.setVoltage(counter, false);   // if you pass true, the output value will also be written
                                        // to the MCP4725's internal non-volatile memory, meaning
                                        // that the DAC will retain the current voltage output
                                        // after power-down or reset
    }
    for (counter = 4095; counter > 0; counter--)
    {
      dac.setVoltage(counter, false);
    }
}
