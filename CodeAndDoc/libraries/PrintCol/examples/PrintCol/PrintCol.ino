#include "PrintCol.h"
#include <limits.h>

void setup()
{
  Serial.begin(9600);

  /* PROVIDE A PRINT OBJECT */
  
  Serial.print("|"); printCol(314, DEC, 11, Serial);  Serial.println("|");  // Serial object
  Serial.print("|"); printCol(3.14, 2, 11, Serial);   Serial.println("|");  // Serial object

  
  /* INTEGER NUMBERS */
  
  // HEX, OCT and BIN tests
  Serial.print("|"); printCol(0x1234, HEX);           Serial.println("|");
  Serial.print("|"); printCol(0173, OCT);             Serial.println("|");
  Serial.print("|"); printCol(0b11110000, BIN);       Serial.println("|");  // for BIN the column width adjustment is ignored, 
                                                                            // and the stock print() function is employed
  
  // minWidth examples
  Serial.print("|"); printCol(-1234567890, DEC, 0);   Serial.println("|");  // minWidth 0 means use the minimum size
  Serial.print("|"); printCol(1234567890);            Serial.println("|");  // default minWidth is 11
  Serial.print("|"); printCol(1234567890, DEC, 25);   Serial.println("|");  // minWidth has an upper limit of 20

  // long
  long lTest = LONG_MIN;
  unsigned long ulTest = ULONG_MAX;
  Serial.print("|"); printCol(lTest);                 Serial.println("|");
  Serial.print("|"); printCol(ulTest);                Serial.println("|");
  
  // int
  int iTest = INT_MIN;
  unsigned int uiTest = UINT_MAX;
  Serial.print("|"); printCol(iTest);                 Serial.println("|");
  Serial.print("|"); printCol(uiTest);                Serial.println("|");

  // char
  char cTest = CHAR_MIN;
  unsigned char ucTest = UCHAR_MAX;
  Serial.print("|"); printCol(cTest);                 Serial.println("|");
  Serial.print("|"); printCol(ucTest);                Serial.println("|");


  /* FLOATING POINT NUMBERS */
  
  // minWidth and precision examples
  Serial.print("|"); printCol(10.123456789, 10, 0);   Serial.println("|");  // precision is limited to 7 and minWidth 0 means use the minimum size
  Serial.print("|"); printCol(10.123456789);          Serial.println("|");  // default minWidth is 11 and default precision is 2
  Serial.print("|"); printCol(10.123456789, 2, 25);   Serial.println("|");  // minWidth has an upper limit of 20
  Serial.print("|"); printCol(10.123456789, 0, 5);    Serial.println("|");  // precision of 0

  // Switch threshold betwenn normal and scientific notation
  Serial.print("|"); printCol(9.999999e9, 7, 20);     Serial.println("|");  // 9999999000.0000000
  Serial.print("|"); printCol(9.9999999e9, 7, 20);    Serial.println("|");  // 1.0000000e+10
  Serial.print("|"); printCol(0.0000001, 7, 20);      Serial.println("|");  // 0.0000001
  Serial.print("|"); printCol(0.00000009, 7, 20);     Serial.println("|");  // 9.0000000e-08

  // Nan
  Serial.print("|"); printCol(sqrt(-1.0), 7, 20);     Serial.println("|");  // nan
  
  // Limits
  Serial.print("|"); printCol(-1e-46, 7, 20);         Serial.println("|");  // -0.0000000e+00
  Serial.print("|"); printCol(1e-46, 7, 20);          Serial.println("|");  // 0.0000000e+00
  Serial.print("|"); printCol(-3.4028236e+38, 7, 20); Serial.println("|");  // -inf
  Serial.print("|"); printCol(3.4028236e+38, 7, 20);  Serial.println("|");  // inf

  // Limits of the Arduino AVR print()
  // see:  printFloat(double number, uint8_t digits)
  // from: https://github.com/arduino/ArduinoCore-avr/blob/master/cores/arduino/Print.cpp
  Serial.print("|"); Serial.print(4294967167.0);      Serial.println("|");  // that's rounded to 4294967040.00 which is the limit of print()
  Serial.print("|"); Serial.print(4294967168.0);      Serial.println("|");  // that's beyond the limit and returns ovf
  Serial.print("|"); Serial.print(-4294967167.0);     Serial.println("|");  // that's rounded to -4294967040.00 which is the limit of print()
  Serial.print("|"); Serial.print(-4294967168.0);     Serial.println("|");  // that's beyond the limit and returns ovf
}

void loop()
{
  
}
