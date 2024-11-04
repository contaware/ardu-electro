/*
  DS3231 RTC module with I2C support
 
  VDD supply is 2.3V – 5.5V.
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
  Wire.begin();
  //setDateTime();
  Serial.begin(9600);
}

void loop()
{
  printDateTime();
  printTemp();
  Serial.print("Single reg read of year=");
  Serial.println(bcdToDec(readRegister(0x06)));
  delay(1000);
}
