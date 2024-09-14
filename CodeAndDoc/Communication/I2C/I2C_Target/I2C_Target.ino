/*
  I2C (Inter-Integrated Circuit) / TWI (Two Wire Interface) communication between
  Arduinos, this is the Sketch for the Arduino acting as Target.

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

void setup()
{
  // Init Serial
  Serial.begin(9600);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!
  delay(5000);      // for ESP32 and some other MCUs a delay() is needed, otherwise
                    // the messages generated in setup() can't be seen!

  Serial.println("I2C Target Test: wait that the Controller sends/asks something...");

  // Join I2C bus with given address
  Wire.begin(I2C_ADDR);

  // The functions registered by onReceive and onRequest are interrupt
  // handlers, do not execute anything time consuming within them!
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

void receiveEvent(int howMany)
{
  while (Wire.available() > 0)
  {
    char c = Wire.read();
    Serial.print(c);
  }
}

void requestEvent()
{ 
  Wire.write(reqSampleMsg.c_str());
}

void loop()
{

}
