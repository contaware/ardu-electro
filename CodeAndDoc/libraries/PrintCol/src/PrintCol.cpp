/*
  Numbers print with column width adjustment for tabular output.
  This library provides support for printing right-aligned numbers in 
  columns. Default output is to Serial, but can be customized.
*/

#include "PrintCol.h"

/*
  DO NOT lower that define under 21 because the AVR dtostrf() and 
  dtostre() used in printCol(double num, ...) do not have buffer size
  checks like snprintf().
*/
#define PRINTCOL_BUF_SIZE                 21


void printCol(char num, unsigned char base/*=DEC*/, unsigned char minWidth/*=11*/, Print& p/*=Serial*/)
{
  printCol((long)num, base, minWidth, p);
}

void printCol(signed char num, unsigned char base/*=DEC*/, unsigned char minWidth/*=11*/, Print& p/*=Serial*/)
{
  printCol((long)num, base, minWidth, p);
}

void printCol(unsigned char num, unsigned char base/*=DEC*/, unsigned char minWidth/*=11*/, Print& p/*=Serial*/)
{
  printCol((unsigned long)num, base, minWidth, p);
}

void printCol(int num, unsigned char base/*=DEC*/, unsigned char minWidth/*=11*/, Print& p/*=Serial*/)
{
  printCol((long)num, base, minWidth, p);
}

void printCol(unsigned int num, unsigned char base/*=DEC*/, unsigned char minWidth/*=11*/, Print& p/*=Serial*/)
{
  printCol((unsigned long)num, base, minWidth, p);
}

void printCol(long num, unsigned char base/*=DEC*/, unsigned char minWidth/*=11*/, Print& p/*=Serial*/)
{
  char fmt[7]; // that's the correct size in case that minWidth has 3 digits
  char buf[PRINTCOL_BUF_SIZE];

  if (minWidth > (PRINTCOL_BUF_SIZE - 1))
    minWidth = (PRINTCOL_BUF_SIZE - 1);

  if (base == BIN)
  {
    // snprintf does not support binary output and
    // a binary would also be too long for our buf
    p.print(num, base);
    return;
  }
  else if (base == HEX)
    sprintf(fmt, "%%%ulX", minWidth);
  else if (base == OCT)
    sprintf(fmt, "%%%ulo", minWidth);
  else // DEC
    sprintf(fmt, "%%%uld", minWidth);

  snprintf(buf, PRINTCOL_BUF_SIZE, fmt, num); // no more than PRINTCOL_BUF_SIZE chars (including the NUL char) are converted to buf
  p.write(buf);
}

void printCol(unsigned long num, unsigned char base/*=DEC*/, unsigned char minWidth/*=11*/, Print& p/*=Serial*/)
{
  char fmt[7]; // that's the correct size in case that minWidth has 3 digits
  char buf[PRINTCOL_BUF_SIZE];

  if (minWidth > (PRINTCOL_BUF_SIZE - 1))
    minWidth = (PRINTCOL_BUF_SIZE - 1);

  if (base == BIN)
  {
    // snprintf does not support binary output and
    // a binary would also be too long for our buf
    p.print(num, base);
    return;
  }
  else if (base == HEX)
    sprintf(fmt, "%%%ulX", minWidth);
  else if (base == OCT)
    sprintf(fmt, "%%%ulo", minWidth);
  else // DEC
    sprintf(fmt, "%%%ulu", minWidth);

  snprintf(buf, PRINTCOL_BUF_SIZE, fmt, num); // no more than PRINTCOL_BUF_SIZE chars (including the NUL char) are converted to buf
  p.write(buf);
}

void printCol(double num, unsigned char precision/*=2*/, unsigned char minWidth/*=11*/, Print& p/*=Serial*/)
{
  char buf[PRINTCOL_BUF_SIZE];

  // minWidth limit to avoid overflowing buf
  if (minWidth > (PRINTCOL_BUF_SIZE - 1))
    minWidth = (PRINTCOL_BUF_SIZE - 1);

  // Precision limit to avoid overflowing buf
  if (precision > 7)
      precision = 7;

  // Range check to avoid showing nothing when numAbs is too small
  // and to avoid overflowing buf when numAbs is really big
  double numAbs = fabs(num);
  if (numAbs >= 0.0000001 && numAbs <= 9.999999e9)
  {
#if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_MEGAAVR)
    // minWidth is the sum of all chars in "[-]d.ppp", while precision is the sum of all p
    dtostrf(num, minWidth, precision, buf);
#else
    #if defined(ARDUINO_ARCH_SAMD)   // not necessary for ARDUINO_ARCH_SAM (Arduino Due)
    asm(".global _printf_float");    // force the compiler to link floating point support
    #endif
    char fmt[8]; // that's the correct size in case that minWidth has 3 digits and precision 1 digit
    sprintf(fmt, "%%%u.%uf", minWidth, precision);
    snprintf(buf, PRINTCOL_BUF_SIZE, fmt, num); // no more than PRINTCOL_BUF_SIZE chars (including the NUL char) are converted to buf
#endif
  }
  else
  {
#if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_MEGAAVR)
    // minWidth is the sum of all chars in "[-]d.pppe±dd", while precision is the sum of all p
    dtostre(num, buf, precision, 0); // last parameter could be set to DTOSTR_UPPERCASE, 
                                     // but let's leave the default: e, nan, inf

    // Align to the right
    int srcPos = strlen(buf);        // starts by pointing to the NUL termination
    int dstPos = minWidth;           // starts by pointing to the last position which gets the NUL
    if (srcPos < dstPos)             // move the string up?
    {
      while (srcPos >= 0)            // continue to copy until reaching the first char
        buf[dstPos--] = buf[srcPos--];
      while (dstPos >= 0)            // fill with spaces until reaching the first position
        buf[dstPos--] = ' ';
    }
#else
    #if defined(ARDUINO_ARCH_SAMD)   // not necessary for ARDUINO_ARCH_SAM (Arduino Due)
    asm(".global _printf_float");    // force the compiler to link floating point support
    #endif
    char fmt[8]; // that's the correct size in case that minWidth has 3 digits and precision 1 digit
    sprintf(fmt, "%%%u.%ue", minWidth, precision);
    snprintf(buf, PRINTCOL_BUF_SIZE, fmt, num); // no more than PRINTCOL_BUF_SIZE chars (including the NUL char) are converted to buf
#endif
  }
  p.write(buf);
}
