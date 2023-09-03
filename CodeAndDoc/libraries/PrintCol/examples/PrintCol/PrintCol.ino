/*
  Numbers print with column width adjustment for tabular output.
  This library provides support for printing right-aligned numbers in 
  columns. Default output is to Serial, but can be customized.
*/
#include "PrintCol.h"
#include <limits.h>

void setup()
{
  /* SERIAL INIT */
  Serial.begin(9600);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!
  delay(5000);      // for ESP32 and some other MCUs a delay() is needed, otherwise
                    // the messages generated in setup() can't be seen!
  Serial.print(F("Compiled: ")); Serial.print(F(__DATE__)); Serial.print(F(", ")); Serial.println(F(__TIME__)); Serial.println();


  /* PROVIDE A PRINT OBJECT */
  Serial.println("PROVIDE A PRINT OBJECT");
  
  Serial.print("|"); 
  printCol(314, DEC, 11, Serial);
  Serial.print("|");
  printCol(3.14, 2, 11, Serial);
  Serial.print("|");
  printCol(3.14e15, 2, 11, Serial);
  Serial.println("|");

  
  /* INTEGER NUMBERS */
  Serial.println(); Serial.println("INTEGER NUMBERS");
  
  // HEX, OCT and BIN
  Serial.print("|"); printCol(0x1234ABCD, HEX);       Serial.println("|");
  Serial.print("|"); printCol(0173, OCT);             Serial.println("|");
  Serial.print("|"); printCol(0b11110000, BIN);       Serial.println("|");
  Serial.print("|"); printCol(ULONG_MAX, BIN);        Serial.println("|");
  Serial.print("|"); printCol(ULONG_LONG_MAX, BIN);   Serial.println("|");
  
  // minWidth
  Serial.print("|"); printCol(-1234567890, DEC, 0);   Serial.println("|");  // minWidth 0 means use the minimum size
  Serial.print("|"); printCol(1234567890);            Serial.println("|");  // default minWidth is 11
  Serial.print("|"); printCol(12345678987654321, DEC, 25); Serial.println("|");
  Serial.print("|"); printCol(-12345678987654321, DEC, 25); Serial.println("|");
  Serial.print("|"); printCol(-1234, DEC, 40);        Serial.println("|");  // for 8-bit to 32-bit numbers minWidth has an upper limit of 32
  Serial.print("|"); printCol(-1234LL, DEC, 80);      Serial.println("|");  // for 64-bit numbers minWidth has an upper limit of 64
  
  // long long
  long long llTest = LONG_LONG_MIN;
  unsigned long long ullTest = ULONG_LONG_MAX;
  Serial.print("|"); printCol(sizeof(llTest));        Serial.println("|          // sizeof(long long)");
  Serial.print("|"); printCol(llTest);                Serial.println("| // LONG_LONG_MIN");
  Serial.print("|"); printCol(ullTest);               Serial.println("| // ULONG_LONG_MAX");
  
  // long
  long lTest = LONG_MIN;
  unsigned long ulTest = ULONG_MAX;
  Serial.print("|"); printCol(sizeof(lTest));         Serial.println("|          // sizeof(long)");
  Serial.print("|"); printCol(lTest);                 Serial.println("|          // LONG_MIN");
  Serial.print("|"); printCol(ulTest);                Serial.println("|          // ULONG_MAX");

  // int
  int iTest = INT_MIN;
  unsigned int uiTest = UINT_MAX;
  Serial.print("|"); printCol(sizeof(iTest));         Serial.println("|          // sizeof(int)");
  Serial.print("|"); printCol(iTest);                 Serial.println("|          // INT_MIN");
  Serial.print("|"); printCol(uiTest);                Serial.println("|          // UINT_MAX");

  // short
  short shTest = SHRT_MIN;
  unsigned short ushTest = USHRT_MAX;
  Serial.print("|"); printCol(sizeof(shTest));        Serial.println("|          // sizeof(short)");
  Serial.print("|"); printCol(shTest);                Serial.println("|          // SHRT_MIN");
  Serial.print("|"); printCol(ushTest);               Serial.println("|          // USHRT_MAX");

  // char
  // Note: char is signed on AVR and unsigned on ARM, but differs from both signed char (int8_t)
  //       and unsigned char (uint8_t or byte) because C/C++ defines char, signed char and 
  //       unsigned char as three distinct data types. Never do math operations with char
  //       because you do not know whether it is signed or unsigned!
  char cTest = CHAR_MIN;
  signed char scTest = SCHAR_MIN;
  unsigned char ucTest = UCHAR_MAX;
  Serial.print("|"); printCol(sizeof(cTest));         Serial.println("|          // sizeof(char)");
  Serial.print("|"); printCol(cTest);                 Serial.println("|          // CHAR_MIN");
  Serial.print("|"); printCol(scTest);                Serial.println("|          // SCHAR_MIN");
  Serial.print("|"); printCol(ucTest);                Serial.println("|          // UCHAR_MAX");

  
  /* FLOATING POINT NUMBERS */
  Serial.println(); Serial.println("FLOATING POINT NUMBERS");

  // float
  float fTest = 10.123456789;
  Serial.print("|"); printCol(sizeof(fTest));         Serial.println("|          // sizeof(float)");
  Serial.print("|"); printCol(fTest, 7);              Serial.println("|");
  
  // double
  double dfTest = 10.123456789;
  Serial.print("|"); printCol(sizeof(dfTest));        Serial.println("|          // sizeof(double)");
  Serial.print("|"); printCol(dfTest, 7);             Serial.println("|");
  
  // minWidth and precision examples
  Serial.print("|"); printCol(10.123456789, 10, 0);   Serial.println("|");  // precision is limited to 7 and minWidth 0 means use the minimum size
  Serial.print("|"); printCol(10.123456789);          Serial.println("|");  // default minWidth is 11 and default precision is 2
  Serial.print("|"); printCol(10.123456789, 2, 25);   Serial.println("|");  // minWidth has an upper limit of 20
  Serial.print("|"); printCol(10.123456789, 0, 5);    Serial.println("|");  // precision of 0

  // Switch threshold between normal and scientific notation
  Serial.print("|"); printCol(9.999999e9, 7, 20);     Serial.println("|");
  Serial.print("|"); printCol(9.9999999e9, 7, 20);    Serial.println("|");
  Serial.print("|"); printCol(0.0000001, 7, 20);      Serial.println("|");
  Serial.print("|"); printCol(0.00000009, 7, 20);     Serial.println("|");
  Serial.print("|"); printCol(0.0, 7, 20);            Serial.println("|");

  // Nan
  Serial.print("|"); printCol(sqrt(-1.0), 7, 20);     Serial.println("|");  // nan

  // Limits of stock print()
  Serial.print("|"); Serial.print(4294967040.0);      Serial.println("|");  // that's the positive limit of print()
  Serial.print("|"); Serial.print(4294967168.0);      Serial.println("|");  // that's beyond the limit and returns ovf (in case of 32-bit float this is the first number not rounded to 4294967040.0)
  Serial.print("|"); Serial.print(-4294967040.0);     Serial.println("|");  // that's the negative limit of print()
  Serial.print("|"); Serial.print(-4294967168.0);     Serial.println("|");  // that's beyond the limit and returns ovf (in case of 32-bit float this is the first number not rounded to -4294967040.0)
}

void loop()
{
  
}
