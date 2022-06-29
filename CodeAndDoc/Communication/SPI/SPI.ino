/* 
  SPI

  Transactional SPI using SPI.beginTransaction() was introduced in 2014, it was not available
  in the early version of the Arduino. What the SPI.beginTransaction() does is to allow you
  to set unique SPI settings (through SPISettings object) for your application, even if other
  devices use different settings. SPI.beginTransaction() behaves like a locking mechanism to 
  gain the exclusive use of the SPI bus, and therefore requires SPI.endTransaction() to
  release the bus for others to access it. SPI.beginTransaction() provides better cross-device
  compatibility and solves software conflicts by allowing multiple SPI devices to properly 
  share the SPI bus. You should therefore use the SPI.beginTransaction() in your SPI sketch.
  You should also using SPISettings object to configure your SPI communication interface, and
  not use the deprecated methods such as SPI.setBitOrder(), SPI.setClockDivider() and 
  SPI.setDataMode() for setting up the SPI configuration.
*/
#include <SPI.h>

const int CS_PIN = 7;                         // we can also use the default SS (slave select) pin which on UNO is 10

void setup()
{
  // For the default SS (slave select) the following two operations are
  // also done in SPI.begin(), but it doesn't harm to do it two times
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);
  
  SPI.begin();                                // initialize SPI to the default of: speed 4000000, MSBFIRST and SPI_MODE0

  sendIt();
  receiveIt();

  SPI.end();                                  // do not call this if using SPI in loop()
}

void loop()
{

}

void sendIt()
{
  SPISettings mySettings(1000000, MSBFIRST, SPI_MODE0);
  SPI.beginTransaction(mySettings);
    
  digitalWrite(CS_PIN, LOW);                  // activate the CS line
  
  SPI.transfer(0x23);                         // send a byte
  SPI.transfer16(0x1113);                     // send a word
  byte buf[4] = {0,1,2,3};
  SPI.transfer(buf, 4);                       // send 4 bytes (no values returned)

  digitalWrite(CS_PIN, HIGH);                 // de-activate the CS line
  
  SPI.endTransaction();
}

void receiveIt()
{
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));

  digitalWrite(CS_PIN, LOW);                  // activate the CS line

  SPI.transfer(0x23);                         // for example a 0x23 tells the slave to transmit a byte and a word
  byte receivedVal = SPI.transfer(0);         // send dummy data to receive the byte
  uint16_t receivedVal16 = SPI.transfer16(0); // send dummy data to receive the word

  digitalWrite(CS_PIN, HIGH);                 // de-activate the CS line
  
  SPI.endTransaction();
}
