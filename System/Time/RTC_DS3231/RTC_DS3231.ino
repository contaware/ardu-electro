/*
  DS3231 precision RTC module (ZS-042) with I2C support
 
  - VDD supply is 2.3V – 5.5V (for the AT24C32 it is 2.7V - 5.5V). 
    To ensure that the time is preserved in the event of a power cut, 
    operate the module with a rechargeable LIR2032 buffer battery 
    or insert a non-rechargeable CR2032 button cell, but pay attention:

    Battery    3.3V supply                5V supply
    -------    -----------                ---------
    CR2032     Battery not affected	      Disable charging circuit
    LIR2032    Battery not affected       Disable charging circuit, or
               (charging does not work)   make sure 5V is actually 4.7V

    Disable charging circuit = remove 201 resistor above the SCL label.

  - The DS3231 is the most accurate of the commonly available chips. 
    It has an accuracy of ±3.5ppm, which translates into: 
    ±3.5ppm * 24 * 60 * 60 = ±3.5/1000000 * 86400 = ±0.3s per day

  - DS3231's I2C address is fixed at 0x68 and the module has also an 
    AT24C32 EEPROM with a default I2C address of 0x57 that can be set 
    to 0x50-0x57. The EEPROM doesn't really have anything to do with 
    the RTC, but it can be used to store any non-volatile data. 
    The SDA and SCL lines have 4.7kΩ pull-up resistors.

  - 32K	is the 32kHz oscillator output which can be used as a clock 
    reference. SQW is usually used as an interrupt output, but it can 
    also be programmed as a square wave output.
*/
#include "Wire.h"
#define DS3231_ADDR 0x68 // 0b1101000

byte decToBcd(byte val)
{
  // Convert normal decimal numbers to binary coded decimal
  return ( (val/10*16) + (val%10) );
}

byte bcdToDec(byte val)
{
  // Convert binary coded decimal to normal decimal numbers
  return ( (val/16*10) + (val%16) );
}

void setDateTime()
{
  byte second = 13; //0-59
  byte minute = 12; //0-59
  byte hour = 11; //0-23
  byte weekDay = 5; //1-7
  byte monthDay = 13; //1-31
  byte month = 11; //1-12
  byte year = 74; //0-99
  
  Wire.beginTransmission(DS3231_ADDR);
  Wire.write(0x00);
  Wire.write(decToBcd(second));
  Wire.write(decToBcd(minute));
  Wire.write(decToBcd(hour));
  Wire.write(decToBcd(weekDay));
  Wire.write(decToBcd(monthDay));
  Wire.write(decToBcd(month));
  Wire.write(decToBcd(year));
  Wire.write(0x00);
  Wire.endTransmission();
}

void printDateTime()
{
  // Read
  Wire.beginTransmission(DS3231_ADDR);
  Wire.write(0x00);
  Wire.endTransmission();
  Wire.requestFrom(DS3231_ADDR, 7); // request 7 bytes from peripheral device DS3231_ADDR
  int second = bcdToDec(Wire.read());
  int minute = bcdToDec(Wire.read());
  int hour = bcdToDec(Wire.read() & 0b111111); //24 hour time
  int weekDay = bcdToDec(Wire.read()); //0-6 -> sunday – Saturday
  int monthDay = bcdToDec(Wire.read());
  int month = bcdToDec(Wire.read());
  int year = bcdToDec(Wire.read());

  // Print
  Serial.print(monthDay);
  Serial.print(".");
  Serial.print(month);
  Serial.print(".");
  Serial.print(year);
  Serial.print(" ");
  Serial.print(hour);
  Serial.print(":");
  Serial.print(minute);
  Serial.print(":");
  Serial.println(second);
}

byte readRegister(byte reg)
{
  Wire.beginTransmission(DS3231_ADDR);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom(DS3231_ADDR, 1);
  return Wire.read();
}

void writeRegister(byte reg, byte data)
{
  Wire.beginTransmission(DS3231_ADDR);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission();
}

void printTemp()
{
  union int16_byte {
      int i;
      byte b[2];
  } rtcTemp;
  Wire.beginTransmission(DS3231_ADDR);
  Wire.write(0x11);
  Wire.endTransmission();
  Wire.requestFrom(DS3231_ADDR, 2);
  rtcTemp.b[1] = Wire.read(); 
  rtcTemp.b[0] = Wire.read();
  long tempC100 = (rtcTemp.i >> 6) * 25; //degrees celsius times 100
  Serial.print("Temp ");
  Serial.print(tempC100 / 100);
  Serial.print('.');
  Serial.print(abs(tempC100 % 100));
  Serial.println(" °C");
}

void setup()
{
  // Init Serial
  Serial.begin(9600);
  
  // Init I2C
  Wire.begin();
  //setDateTime(); // uncomment to set date/time
}

void loop()
{
  printDateTime();
  printTemp();
  Serial.print("Single reg read of year=");
  Serial.println(bcdToDec(readRegister(0x06)));
  delay(1000);
}
