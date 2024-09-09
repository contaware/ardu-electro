/*
  MT6701 Hall based angle position encoder sensor
  
  - VDD supply is 3.3V - 5V.

  - Can be interfaced with I2C to get a 14-bit resolution or there
    is also an analog output which has a resolution of 12-bit.

  ATTENTION: the provided diametrically magnetized magnet has to be 
             glued to a stick and turned with care above the chip.
             Pay attention because it easily gets attracted by the
             nearby contacts!!
*/
#include <Wire.h>

#define MT6701_ADDR      0x06

int readCount()
{
  Wire.beginTransmission(MT6701_ADDR);
  Wire.write(0x03);                   // starting register ANGLE_H
  Wire.endTransmission(false);        // end transmission, but keep the I2C bus active
  Wire.requestFrom(MT6701_ADDR, 2);   // request two bytes
  if (Wire.available() < 2)
    return -1;
  int angle_h = Wire.read();
  int angle_l = Wire.read();
  return (angle_h << 6) | angle_l;    // returns value 0..16383
}

void setup()
{
  Serial.begin(9600);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!
  delay(5000);      // for ESP32 and some other MCUs a delay() is needed, otherwise
                    // the messages generated in setup() can't be seen!

  // I2C begin
  Wire.begin();
}

void loop()
{
  Serial.print(readCount() * 360.0 / 16384.0, 1);
  Serial.println("°");
  delay(300);
}
