/* 
  Read/Write I2C EEPROM chips from the 24LC01-24LC512 series (old 24C01-24C512)

  - The 24LCxxx operate at 2.5V-5.5V, while the older 24Cxxx operate at 4.5-5.5V.

  - As usual SDA and SCL require pull-ups of 10kΩ for 100kHz and 2kΩ for 400kHz.

  - Connect the chip to the Arduino like:
        ---u---
    A0 |1     8| VCC = 1.7V to 5.5V
    A1 |2     7| WP = write protect pin
    A2 |3     6| SCL
   GND |4     5| SDA
        -------
        24LCxxx

  - The WP pin must be connected to either GND or VCC. If tied to GND, write 
    operations are enabled. If tied to VCC, write operations are inhibited but 
    read operations are not affected. The library can also control that pin if
    connected to an Arduino digital pin, please check the library documentation.
*/
#include "Wire.h"
#include "I2C_eeprom.h" // https://github.com/RobTillaart/I2C_EEPROM

/********************* The I2C address of the EEPROM **************************
- 24LC01-24LC16 always use 0x50. 
  Notes: 1. A0,A1,A2 can be left floating or tied to either GND or VCC.
         2. 0x50..0x57 I2C address range is employed to address the blocks!

- 24LC32-24LC256 use an address from the 0x50..0x57 range depending on A0,A1,A2.
  Warning: A0,A1,A2 must be connected to either GND or VCC.
*/
const uint8_t I2C_EEPROM_ADDR = 0x50;

/******************************************************************************
 ********************* REMEMBER TO SET YOUR DEVICE SIZE  **********************
 ******************************************************************************
define                  Size (bytes)  Page Size (bytes)  Structure
------                  ------------  -----------------  ---------
I2C_DEVICESIZE_24LC512  65536         128                64K x 8-bit
I2C_DEVICESIZE_24LC256  32768          64                32K x 8-bit
I2C_DEVICESIZE_24LC128  16384          64                16K x 8-bit
I2C_DEVICESIZE_24LC64    8192          32                8K  x 8-bit
I2C_DEVICESIZE_24LC32    4096          32                4K  x 8-bit
I2C_DEVICESIZE_24LC16    2048          16                256 x 8-bit (8 blocks)
I2C_DEVICESIZE_24LC08    1024          16                256 x 8-bit (4 blocks)
I2C_DEVICESIZE_24LC04     512          16                256 x 8-bit (2 blocks)
I2C_DEVICESIZE_24LC02     256           8                256 x 8-bit
I2C_DEVICESIZE_24LC01     128           8                128 x 8-bit
*/
I2C_eeprom ee(I2C_EEPROM_ADDR, I2C_DEVICESIZE_24LC16);

void printlnHEX(uint8_t value)
{
  if (value < 16)
    Serial.print("0x0");
  else
    Serial.print("0x");
  Serial.println(value, HEX);
}

void writeEEPROM(uint16_t memAddr, uint8_t value)
{
  // writeByte() returns the value of I2C's endTransmission() function,
  // which is: 0=OK, otherwise=error.
  int ret;
  if ((ret = ee.writeByte(memAddr, value)) != 0)
  {
    Serial.print("ERROR: can't write to EEPROM, code=");
    Serial.println(ret);
    while (true);
  }
}
  
void setup()
{
  // Init Serial
  Serial.begin(9600);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!
  delay(5000);      // for ESP32 and some other MCUs a delay() is needed, otherwise
                    // the messages generated in setup() can't be seen!

  Serial.print("I2C EEPROM Library Version: "); Serial.println(I2C_EEPROM_VERSION);

  // Init random
  randomSeed(analogRead(A0));
  
  // Before any write/read operations the library waits a maximum of 5ms + 
  // the here given amount of extra write cycle time in ms.
  // Example: some older devices have a write cycle of 10ms, so set an additional 5ms.
  ee.setExtraWriteCycleTime(5); // in ms
  
  // I2C begin
  Wire.begin();
  //Wire.setClock(400000); // default is standard mode=100kHz, uncomment for fast mode=400kHz

  // EEPROM begin
  ee.begin();
  if (!ee.isConnected())
  {
    Serial.println("ERROR: can't find EEPROM!");
    while (true);
  }

  // Write 3 random bytes
  uint16_t memAddr1 = 0x05; uint8_t value1 = random(0, 256);
  Serial.print("Write1="); printlnHEX(value1);
  writeEEPROM(memAddr1, value1);
  uint16_t memAddr2 = 0x3C; uint8_t value2 = random(0, 256);
  Serial.print("Write2="); printlnHEX(value2);
  writeEEPROM(memAddr2, value2);
  uint16_t memAddr3 = 0x7D; uint8_t value3 = random(0, 256);
  Serial.print("Write3="); printlnHEX(value3);
  writeEEPROM(memAddr3, value3);
  
  // Read them back
  uint8_t value1Readback = ee.readByte(memAddr1);
  Serial.print("Read1="); printlnHEX(value1Readback);
  uint8_t value2Readback = ee.readByte(memAddr2);
  Serial.print("Read2="); printlnHEX(value2Readback);
  uint8_t value3Readback = ee.readByte(memAddr3);
  Serial.print("Read3="); printlnHEX(value3Readback);
  if (value1 != value1Readback || value2 != value2Readback || value3 != value3Readback)
    Serial.println("Did you set the correct device size?");
}

void loop()
{

}
