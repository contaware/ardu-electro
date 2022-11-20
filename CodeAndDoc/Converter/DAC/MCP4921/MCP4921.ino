/*
  MCP4921 12-Bit SPI DAC chip
 
  - VDD supply is 2.7V - 5.5V.
  
  - External Voltage Reference Input 0 - VDD.

  - Selectable Unity or 2x Gain Output.

  - SPI Interface with 20 MHz Clock Support.
*/
#include "MCP_DAC.h"
MCP4921 dac; // HW SPI, for SW SPI use MCP4921 dac(DAC_MOSI_PIN, DAC_SCK_PIN);  
#define DAC_CS_PIN          10
#define USE_FAST_WRITE      true
#define MCU_MAX_SPI_SPEED   8000000 // Atmega328P max SPI speed is F_CPU / 2 -> 8000000
                                    // (setting a value greater than supported will use the max speed)
void setup()
{
  // Init Serial
  Serial.begin(9600);
  
  // Init DAC
  dac.begin(DAC_CS_PIN);
  dac.setSPIspeed(MCU_MAX_SPI_SPEED);

  // Print Info
  Serial.print("MCP_DAC_LIB_VERSION: ");
  Serial.println(MCP_DAC_LIB_VERSION);
  Serial.print("CHANNELS:            ");
  Serial.println(dac.channels());
  Serial.print("MAX VALUE:           ");
  Serial.println(dac.maxValue());
  Serial.print("HW SPI:              ");
  Serial.println(dac.usesHWSPI());
  Serial.print("SPI SPEED:           ");
  Serial.println(dac.getSPIspeed()); // library default is 16000000
}

void loop()
{
  uint16_t counter;
  uint16_t maxDacValue = dac.maxValue();
  
  // Run through the full bit scale for a triangle wave
  // Note: with dac.fastWriteA() it reaches 8 Hz otherwise 6 Hz
  for (counter = 0; counter < maxDacValue; counter++)
  {
#if USE_FAST_WRITE == true
    dac.fastWriteA(counter);
#else
    dac.analogWrite(counter);
#endif
  }
  for (counter = maxDacValue; counter > 0; counter--)
  {
#if USE_FAST_WRITE == true
    dac.fastWriteA(counter);
#else
    dac.analogWrite(counter);
#endif
  }
}
