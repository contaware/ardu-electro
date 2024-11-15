/* 
  SPI (Serial Peripheral Interface) loopback test 
  (connect MISO with MOSI)

  - We use the terms:
    * Main (=old term was Master).
    * Sub (=old term was Slave).
    Note: some Arduinos use Controller for Main and Peripheral for Sub.

  - The Arduino SPI library can only act as a Main. There is always one 
    Main and one or multiple Subs, each one selected by a chip select 
    line. The transfer is based on a simultaneous send and receive, the 
    Main generates the clock, places data on MOSI and receives data on 
    MISO.

  - The SPI pins are:
    * SCK (Serial Clock).
    * MISO (Main In Sub Out).
      Note: the Sub must release the MISO line (configure it as an input 
            or high impedance) when its chip select line goes high.
    * MOSI (Main Out Sub In).
    * SS (Sub Select), also called Chip Select (CS), this signal is 
      active low.

  - Mode          Clock Polarity      Data Out Edge     Data Capture Edge
    ----          --------------      -------------     -----------------
    SPI_MODE0     SCK idle when 0     Falling           Rising
    SPI_MODE1     SCK idle when 0     Rising            Falling
    SPI_MODE2     SCK idle when 1     Rising            Falling
    SPI_MODE3     SCK idle when 1     Falling           Rising

  - For Atmega328P the allowed SPI speeds are a divider of F_CPU:
    2, 4, 8, 16, 32, 64, 128
*/
#include <SPI.h>

// Define the chip select pin
#ifdef ARDUINO_ARCH_AVR
#define SPI_CS_PIN    SS
#else
#define SPI_CS_PIN    7  // use whatever is free for your platform
#endif
  
// Limit for the Serial Monitor input characters
#define SERIAL_MON_CHARS_LIMIT   16
uint8_t buf[SERIAL_MON_CHARS_LIMIT];

void setup()
{
  // Init Serial
  Serial.begin(9600);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!
  delay(5000);      // for ESP32 and some other MCUs a delay() is needed, otherwise
                    // the messages generated in setup() can't be seen!
  Serial.println("SPI Main Test: type in upper window and press ENTER (or just press ENTER)");

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
  */
#ifndef ARDUINO_ARCH_AVR
  pinMode(SPI_CS_PIN, OUTPUT);
  digitalWrite(SPI_CS_PIN, HIGH);
#endif
  SPI.begin();
}

uint8_t transaction8SPI(uint8_t value)
{
  // SPI Settings for the transaction
  SPISettings mySPISettings(4000000, // constructor finds the fastest possible clock that is less 
                                     // than or equal to the provided clock
                          MSBFIRST,  // MSBFIRST (default) or LSBFIRST which is less common 
                          SPI_MODE0);// SPI_MODE0 (default), SPI_MODE1, SPI_MODE2 or SPI_MODE3

  // SPI.beginTransaction() locks the access to the SPI bus, SPI.endTransaction() 
  // will release it for other parts of the software
  SPI.beginTransaction(mySPISettings);

  // Activate the CS line
  digitalWrite(SPI_CS_PIN, LOW);

  // Send a byte and receive a byte
  // MSBFIRST: first bit7, then bit6, ... , bit0
  // LSBFIRST: first bit0, then bit1, ... , bit7
  uint8_t receivedVal8 = SPI.transfer(value);

  // De-activate the CS line
  digitalWrite(SPI_CS_PIN, HIGH);

  // SPI transaction end
  SPI.endTransaction();

  return receivedVal8;
}

uint16_t transaction16SPI(uint16_t value)
{
  // SPI Settings for the transaction
  SPISettings mySPISettings(4000000, // constructor finds the fastest possible clock that is less 
                                     // than or equal to the provided clock
                          MSBFIRST,  // MSBFIRST (default) or LSBFIRST which is less common 
                          SPI_MODE0);// SPI_MODE0 (default), SPI_MODE1, SPI_MODE2 or SPI_MODE3

  // SPI.beginTransaction() locks the access to the SPI bus, SPI.endTransaction() 
  // will release it for other parts of the software
  SPI.beginTransaction(mySPISettings);

  // Activate the CS line
  digitalWrite(SPI_CS_PIN, LOW);

  // Send a word and receive a word 
  // MSBFIRST: first bit15, then bit14, ... , bit0
  // LSBFIRST: first bit0,  then bit1,  ... , bit15
  uint16_t receivedVal16 = SPI.transfer16(value);
  
  // De-activate the CS line
  digitalWrite(SPI_CS_PIN, HIGH);

  // SPI transaction end
  SPI.endTransaction();

  return receivedVal16;
}

void transactionBufSPI(uint8_t* buf, int bufSize)
{
  // SPI Settings for the transaction
  SPISettings mySPISettings(4000000, // constructor finds the fastest possible clock that is less 
                                     // than or equal to the provided clock
                          MSBFIRST,  // MSBFIRST (default) or LSBFIRST which is less common 
                          SPI_MODE0);// SPI_MODE0 (default), SPI_MODE1, SPI_MODE2 or SPI_MODE3

  // SPI.beginTransaction() locks the access to the SPI bus, SPI.endTransaction() 
  // will release it for other parts of the software
  SPI.beginTransaction(mySPISettings);

  // Activate the CS line
  digitalWrite(SPI_CS_PIN, LOW);
  
  // Send an array of bytes, received data is stored in the buffer in-place
  SPI.transfer(buf, bufSize);

  // De-activate the CS line
  digitalWrite(SPI_CS_PIN, HIGH);

  // SPI transaction end
  SPI.endTransaction();
}

void serialPrintValue(uint8_t value)
{
  if (value == 0)
    Serial.print("<0x00>");     // in case that MISO is low
  else if (value == 0xFF)
    Serial.print("<0xFF>");     // in case that MISO is high
  else
    Serial.print((char)value);  // when MISO and MOSI are connected (loop-back)    
}

void loop()
{
  // Get data from serial monitor's input field
  if (Serial.available())
  {
    String msg;
    msg = Serial.readStringUntil('\n'); // function removes '\n' from serial buffer and does not return a '\n'
    msg.trim();                         // remove CR if terminal is sending one
    if (msg.length() == 0)              // if just pressing ENTER
    {
      // Do 16-bit transaction sending OK
      uint16_t val16 = 'O';
      val16 <<= 8;
      val16 |= 'K';
      uint16_t receivedVal16 = transaction16SPI(val16);

      // Print to Serial
      serialPrintValue((uint8_t)(receivedVal16 >> 8));
      serialPrintValue((uint8_t)receivedVal16);
      Serial.println();
    }
    else if (msg.length() > SERIAL_MON_CHARS_LIMIT)
      Serial.println("Too many characters entered");
    else if (msg.length() == 1)
    {
      // Do 8-bit transaction
      uint8_t receivedVal8 = transaction8SPI((uint8_t)msg[0]);
      
      // Print to Serial
      serialPrintValue(receivedVal8);
      Serial.println();
    }
    else
    {
      // Do buffer transaction
      memcpy(buf, msg.c_str(), msg.length());
      transactionBufSPI(buf, msg.length());

      // Print to Serial
      for (unsigned int i = 0; i < msg.length() ; i++)
        serialPrintValue(buf[i]);
      Serial.println();
    }
  }
}
