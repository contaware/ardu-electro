/*
  MAX7219/MAX7221 driven from SPI
 
  - VDD supply is 5V, chip can supply a maximum of 330mA.

  - The chip drives common-cathode displays. It can drive up to eight 
    7-segment displays or a 8x8 LED matrix.

  - MAX7219 doesn't respect the chip select pin, it will see traffic 
    intended for other devices on the SPI lines. If driving more than 
    one SPI device, use the MAX7221 chip which is fully SPI compliant.

  - Arduino            MAX7219/MAX7221
    MOSI         <->   DIN 
    SCK          <->   CLK
    SPI_CS_PIN   <->   LOAD/CS
*/
#include <SPI.h>

// Define the chip select pin
#ifdef ARDUINO_ARCH_AVR
#define SPI_CS_PIN    SS
#else
#define SPI_CS_PIN    7  // use whatever is free for your platform
#endif

void maxTransfer(uint8_t address, uint8_t value)
{
  SPISettings mySPISettings(4000000, MSBFIRST, SPI_MODE0);
  SPI.beginTransaction(mySPISettings);
  digitalWrite(SPI_CS_PIN, LOW);  // activate the CS line
  SPI.transfer(address);
  SPI.transfer(value);
  digitalWrite(SPI_CS_PIN, HIGH); // de-activate the CS line
  SPI.endTransaction();
}

void setup()
{
  /*
    Init SPI
    By calling SPI.begin() the following happens:
    - SCK:  OUTPUT and LOW
    - MISO: INPUT
    - MOSI: OUTPUT and LOW
    - SS:   OUTPUT and HIGH for AVR only, other platforms do not initialize a chip 
            select. For AVR you can use SS as an OUTPUT for other purposes than 
            selecting a SPI Sub, but never change SS to an INPUT when using SPI,
            as then the SPI hardware may switch to Sub mode and the library does
            not support it.
    Note:
    Setting SS to OUTPUT and HIGH before the SPI.begin() call is not necessary 
    for AVR, but it does not harm to do so.
  */
  pinMode(SPI_CS_PIN, INPUT_PULLUP); // leave it an input, but with pull-up
  pinMode(SPI_CS_PIN, OUTPUT);       // make it an output
  digitalWrite(SPI_CS_PIN, HIGH);    // not necessary for AVR, but maybe for other platforms
  SPI.begin();

  // The chip is in power-saving mode at startup, do a wakeup call
  maxTransfer(0x0C, 0x01);

  // Set brightness
  maxTransfer(0x0A, 0x03); // 0..15

  // Disable BCD decode mode
  maxTransfer(0x09, 0x00);
  
  // Displayed digits
  // 0 = digit 0
  // 1 = digits 0,1
  // 2 = digits 0,1,2
  // ...
  // 7 = digits 0,1,2,3,4,5,6,7
  maxTransfer(0x0B, 7);

  // Test
  // Warning: do not use the test mode as it turns ON all 
  //          LEDs with the maximum brightness!
  for (uint8_t digit = 1 ; digit <= 8 ; digit++)
    maxTransfer(digit, 255);
  delay(3000);
  for (uint8_t digit = 1 ; digit <= 8 ; digit++)
    maxTransfer(digit, 0);
  delay(1000);
}

void loop()
{
  for (uint8_t digit = 1 ; digit <= 8 ; digit++)
  {
    for (uint8_t i = 0 ; i <= 8 ; i++)
    {
      uint8_t segment = 1 << i;
      maxTransfer(digit, segment);
      delay(100);
    }
  }
}
