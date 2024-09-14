/*
  I2C (Inter-Integrated Circuit) / TWI (Two Wire Interface) communication between
  Arduinos, this is the Sketch for the Arduino acting as Controller.

  - We use the terms Controller (=master) and Target (=slave).

  - Some implementations of the Wire library (like the one of AVR) support Multiple 
    Controllers on the same bus, but not every situation is detected. If you want that
    your Arduino project works well, do not use more than a Controller per bus.
  
  - Connect two Arduinos with their SCL, SDA lines and the GND, pay attention to use 
    Arduinos operating at the same logic voltage or use level shifters.
    SCL and SDA are open-drain which means that pull-up resistors need to be attached
    to them. Commonly used values are 2K for the fast speed (400kbps) and 10K for the
    standard speed (100kbps).

  1. The I2C protocol specifies that bytes are always sent most significant bit first.
   
  2. Data transfers may be initiated only when the bus is idle. A bus is considered idle
     if both SDA and SCL are high after a STOP condition. A high to low transition on SDA
     while SCL is high defines a START condition. A low to high transition on SDA while 
     SCL is high defines a STOP condition. A repeated START condition is like a START 
     condition and is used in place of a STOP+START when the Controller wants to start 
     a new communication without losing the bus control.
     For normal data transmission, SDA must remain stable during the high phase of a SCL.
           
  3. The I2C address byte is composed of a 7-bit address (from 8 to 119, the other ones 
     are reserved) identifying a Target and a R/W bit (sent last) which indicates whether
     the Controller sends data to the Target (bit set to 0) or whether the Controller
     wants the Target to send data (bit set to 1). The clock signal is always provided by
     the Controller:
     
     A. Controller sends the address byte and the Target must ACK by pulling SDA low
        on the ninth SCL pulse. After that, more bytes can follow, each one must be ACK 
        by the Target.

     B. Controller sends the address byte and the Target must ACK by pulling SDA low
        on the ninth SCL pulse. Target then sends out a byte which must be ACK by the
        Controller. When the Controller does not need more bytes, it has to NACK by 
        setting SDA high on the ninth SCL pulse.
        Clock stretching: if the Target is not ready to send out data, it is allowed to
        hold SCL low until ready. The Controller will make SCL high and then check it,
        if it's still low then the Controller waits until it goes high.
*/
#include <Wire.h>

// The Wire library uses 7-bit addresses
const uint8_t I2C_ADDR = 0x08;

// Just a sample message
String reqSampleMsg("OK\n");

// AVR Wire library implementation has a packet length limit of 32 bytes,
// for interoperability do that check also for the other Arduino platforms:
#ifndef BUFFER_LENGTH
#define BUFFER_LENGTH 32
#endif

void setup()
{
  // Init Serial
  Serial.begin(9600);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!
  delay(5000);      // for ESP32 and some other MCUs a delay() is needed, otherwise
                    // the messages generated in setup() can't be seen!

  Serial.println("I2C Controller Test: type in upper window and press ENTER (or just press ENTER)");

  // I2C begin
  Wire.begin();
  //Wire.setClock(400000); // default is standard mode=100kHz, uncomment for fast mode=400kHz
}

void writeI2C(String msg)
{
  // I2C Wire library can send packets up to 32 bytes 
  if (msg.length() > BUFFER_LENGTH)
  {
    Serial.print("Message too long, upper limit is: ");
    Serial.print(BUFFER_LENGTH);
    Serial.println(" bytes");
  }
  else
  {
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(msg.c_str());
    Wire.endTransmission();
  }
}

void requestI2C(uint8_t quantity)
{
  // I2C Wire library can request packets up to 32 bytes
  if (quantity > BUFFER_LENGTH)
  {
    Serial.print("Too much data requested, upper limit is: ");
    Serial.print(BUFFER_LENGTH);
    Serial.println(" bytes");
  }
  else
  {
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
  // I2C Wire library can send packets up to 32 bytes 
  if (msg.length() > BUFFER_LENGTH)
  {
    Serial.print("Message too long, upper limit is: ");
    Serial.print(BUFFER_LENGTH);
    Serial.println(" bytes");
  }
  // I2C Wire library can request packets of a maximum of 32 bytes
  else if (quantity > BUFFER_LENGTH)
  {
    Serial.print("Too much data requested, upper limit is: ");
    Serial.print(BUFFER_LENGTH);
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
