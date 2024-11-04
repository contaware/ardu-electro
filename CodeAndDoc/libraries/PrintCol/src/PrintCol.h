/*
  Numbers print with column width adjustment for tabular output.
  This library provides support for printing right-aligned numbers in 
  columns. Default output is to Serial, but can be customized.
*/
#ifndef printcol_h
#define printcol_h

#include "Arduino.h"


/*
- base            
  values:   DEC, HEX, OCT, BIN
  default:  DEC

- minWidth
  range:    0..32
  default:  11

- p
  default:  Serial
  can use:  SoftwareSerial, Serial1, Serial2, Serial3, SerialUSB, ...
*/
extern void printCol(char num, unsigned char base = DEC, unsigned char minWidth = 11, Print& p = Serial);
extern void printCol(signed char num, unsigned char base = DEC, unsigned char minWidth = 11, Print& p = Serial);
extern void printCol(unsigned char num, unsigned char base = DEC, unsigned char minWidth = 11, Print& p = Serial);
extern void printCol(int num, unsigned char base = DEC, unsigned char minWidth = 11, Print& p = Serial);
extern void printCol(unsigned int num, unsigned char base = DEC, unsigned char minWidth = 11, Print& p = Serial);
extern void printCol(long num, unsigned char base = DEC, unsigned char minWidth = 11, Print& p = Serial);
extern void printCol(unsigned long num, unsigned char base = DEC, unsigned char minWidth = 11, Print& p = Serial);

/*
- base            
  values:   DEC, HEX, OCT, BIN
  default:  DEC

- minWidth
  range:    0..64
  default:  11

- p
  default:  Serial
  can use:  SoftwareSerial, Serial1, Serial2, Serial3, SerialUSB, ...
*/
extern void printCol(long long num, unsigned char base = DEC, unsigned char minWidth = 11, Print& p = Serial);
extern void printCol(unsigned long long num, unsigned char base = DEC, unsigned char minWidth = 11, Print& p = Serial);

/*
- precision (number of decimal places to print)
  range:    0..7
  default:  2

- minWidth 
  range:    0..20
  default:  11

- p
  default:  Serial
  can use:  SoftwareSerial, Serial1, Serial2, Serial3, SerialUSB, ...
*/
extern void printCol(double num, unsigned char precision = 2, unsigned char minWidth = 11, Print& p = Serial);

#endif
