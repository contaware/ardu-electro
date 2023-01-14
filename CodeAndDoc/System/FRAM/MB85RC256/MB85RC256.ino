/*
  MB85RC256 module storing 32768 bytes accessed by I2C
 
  - VDD supply is 2.7V – 5.5V.

  - By default the I2C address is 0x50, connecting the A2, A1, A0 pins to VDD 
    and power cycling the chip will adjust the lower three bits of the address.
    For example if A0 is high, the address is 0x51. If A1 and A2 are high, the
    address is 0x56.

  - The WP (Write Protect) pin is used to force write protection so you cannot 
    write to the FRAM. It has an internal pulldown. Bring it to a high voltage
    to turn on the WP.

  - FRAM allows for a lower power usage and a faster write performance. It's 
    excellent for datalogging, unlike Flash or EEPROM there's no pages to worry
    about. Each byte can be read/written more than 1000'000'000'000 times and the
    data can be retained for 200 years without a battery.
*/
#include "Adafruit_FRAM_I2C.h"

Adafruit_FRAM_I2C fram = Adafruit_FRAM_I2C();

void setup()
{
  Serial.begin(9600);
  
  if (fram.begin(MB85RC_DEFAULT_ADDRESS))
    Serial.println("Found I2C FRAM");
  else
  {
    Serial.println("I2C FRAM not identified ... check your connections?");
    while (true);
  }
  
  // Use the first byte as the starts counter
  uint8_t test = fram.read(0x0);
  Serial.print("Restarted "); Serial.print(test); Serial.println(" times");
  fram.write(0x0, test + 1); // inc
  
  // Dump the first 256 bytes of memory
  uint8_t value;
  for (uint16_t a = 0; a < 256; a++)
  {
    value = fram.read(a);
    
    // Row address
    if ((a % 8) == 0)
    {
      Serial.print("\r\n0x");
      if (a < 0x10) Serial.print('0');
      Serial.print(a, HEX);
      Serial.print(": ");
    }

    // Value
    Serial.print("0x"); 
    if (value < 0x10) Serial.print('0');
    Serial.print(value, HEX);
    Serial.print(" ");
  }
}

void loop()
{

}
