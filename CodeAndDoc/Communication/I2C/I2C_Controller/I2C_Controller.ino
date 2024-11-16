/*
  I2C (Inter-Integrated Circuit) / TWI (Two Wire Interface) communication 
  between Arduinos, this is the Sketch for the Arduino acting as Controller.

  - We use the terms Controller (=old term was Master) and Target (=old 
    term was Slave).

  - Some implementations of the Wire library (like the one of AVR) 
    support Multiple Controllers on the same bus, but not every situation 
    is detected. If you want that your Arduino project works well, do not 
    use more than a Controller per bus.
  
  - Connect two Arduinos with their SCL, SDA lines and the GND, pay 
    attention to use Arduinos operating at the same logic voltage or use 
    level shifters. SCL and SDA are open-drain which means that pull-up 
    resistors need to be attached to them. Commonly used values are 2kΩ 
    for the fast speed (400kbps) and 10kΩ for the standard speed (100kbps).

  1. The I2C protocol specifies that bytes are always sent most 
     significant bit first.

  2. Data transfers may be initiated only when the bus is idle. A bus is 
     considered idle if both SDA and SCL are high after a STOP condition. 
     A high to low transition on SDA while SCL is high defines a START 
     condition. A low to high transition on SDA while SCL is high defines 
     a STOP condition. A repeated START condition is like a START 
     condition and is used in place of a STOP+START when the Controller 
     wants to start a new communication without losing the bus control.
     For normal data transmission, SDA must remain stable during the high 
     phase of a SCL.

  3. The I2C address byte is composed of a 7-bit address (from 8 to 119, 
     the other ones are reserved) identifying a Target and a R/W bit 
     (sent last) which indicates whether the Controller sends data to the 
     Target (bit set to 0) or whether the Controller wants the Target to 
     send data (bit set to 1). The clock signal is always provided by
     the Controller:

     A. Controller sends the address byte and the Target must ACK by 
        pulling SDA low on the ninth SCL pulse. After that, more bytes 
        can follow, each one must be ACK by the Target.

     B. Controller sends the address byte and the Target must ACK by 
        pulling SDA low on the ninth SCL pulse. Target then sends out a 
        byte which must be ACK by the Controller. When the Controller 
        does not need more bytes, it has to NACK by setting SDA high on 
        the ninth SCL pulse.
        Clock stretching: if the Target is not ready to send out data, 
        it is allowed to hold SCL low until ready. The Controller will 
        make SCL high and then check it, if it's still low then the 
        Controller waits until it goes high.
*/
#include <Wire.h>

// The Wire library uses 7-bit addresses
const uint8_t I2C_ADDR = 0x08;

// Just a sample message
String reqSampleMsg("OK\n");

// AVR Wire library has a packet length limit of 32 bytes, other
// platforms have a higher limit, for interoperability use 32 bytes:
#define I2C_MAX_PACKET_SIZE   32

void setup()
{
  // Init Serial (leave Serial Monitor open to see all messages)
  Serial.begin(9600); delay(5000); // wait 5s that Serial is ready
  Serial.println("I2C Controller Test: type in upper window and press ENTER (or just press ENTER)");

  // I2C begin
  Wire.begin();
  //Wire.setClock(400000); // default is standard mode=100kHz, uncomment for fast mode=400kHz
}

void writeI2C(String msg)
{
  if (msg.length() > I2C_MAX_PACKET_SIZE)
  {
    Serial.print("Message too long, upper limit is: ");
    Serial.print(I2C_MAX_PACKET_SIZE);
    Serial.println(" bytes");
  }
  else
  {
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(msg.c_str());
    
    /* Blocks until all bytes queued by the write() calls are transmitted.
       Possible return values:
       0: success.
       1: data too long to fit in transmit buffer.
       2: received NACK on transmit of address.
       3: received NACK on transmit of data.
       4: other error.
       5: timeout.
    */
    Wire.endTransmission();
  }
}

void requestI2C(uint8_t quantity)
{
  if (quantity > I2C_MAX_PACKET_SIZE)
  {
    Serial.print("Too much data requested, upper limit is: ");
    Serial.print(I2C_MAX_PACKET_SIZE);
    Serial.println(" bytes");
  }
  else
  {
    // Blocks until all requested bytes are received and queued in rx buffer
    Wire.requestFrom(I2C_ADDR, quantity); // request quantity bytes
    while (Wire.available())
    {
      char c = Wire.read();
      Serial.print(c);
    }
  }
}

void writeAndRequestI2C(String msg, uint8_t quantity)
{
  if (msg.length() > I2C_MAX_PACKET_SIZE)
  {
    Serial.print("Message too long, upper limit is: ");
    Serial.print(I2C_MAX_PACKET_SIZE);
    Serial.println(" bytes");
  }
  else if (quantity > I2C_MAX_PACKET_SIZE)
  {
    Serial.print("Too much data requested, upper limit is: ");
    Serial.print(I2C_MAX_PACKET_SIZE);
    Serial.println(" bytes");
  }
  else
  {
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(msg.c_str());
    Wire.endTransmission(false); // do not send STOP, we want a repeated START
    requestI2C(quantity);
  }
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
      requestI2C(reqSampleMsg.length());
      writeAndRequestI2C("How are you Target?\n", reqSampleMsg.length());
    }
    else
      writeI2C(msg + '\n');
  }
}
