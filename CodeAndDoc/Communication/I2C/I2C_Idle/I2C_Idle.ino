/*
  I2C Idle

  - Upload this sketch and power-up your Arduino with all the I2C peripherals
    attached.
    
  - With a multimeter measure the current drawn when shorting SDA / SCL to GND.

  - By specs the maximum allowed current is 3 mA. According to that choose your
    I2C pull-ups.
*/
#include <Wire.h>

void setup()
{ 
  Wire.begin();
}

void loop()
{

}
