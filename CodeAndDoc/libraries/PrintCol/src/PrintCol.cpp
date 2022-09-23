/*
  Numbers print with column width adjustment for tabular output.
  This library provides support for printing right-aligned numbers in 
  columns. Default output is to Serial, but can be customized.
*/

#include "PrintCol.h"

#define PRINTCOL_UL_BUF_SIZE      33   // 33 needed for 32-bit BIN print
#define PRINTCOL_ULL_BUF_SIZE     65   // 65 needed for 64-bit BIN print
#define PRINTCOL_FLT_BUF_SIZE     21   // DO NOT lower under 21 because
                                       // dtostrf() and dtostre() have
                                       // no buffer size checks!

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

static void printColSign(char signChar, unsigned long num, unsigned char base/*=DEC*/, unsigned char minWidth/*=11*/, Print& p/*=Serial*/)
{
  char buf[PRINTCOL_UL_BUF_SIZE];
  
  // Checks
  if (base < 2)
    base = 2;
  else if (base > 16)
    base = 16;
  if (minWidth > (PRINTCOL_UL_BUF_SIZE - 1))
    minWidth = (PRINTCOL_UL_BUF_SIZE - 1);

  // Init
  int i = PRINTCOL_UL_BUF_SIZE - 1;                   // last position (pointing to NUL)
  int numStart = PRINTCOL_UL_BUF_SIZE - 1 - minWidth; // start position according to minWidth
  buf[i--] = '\0';                                    // NUL terminate

  // Convert
  do
  {
    unsigned long q = num / base;
    char r = num - q * base; // faster than: r = num % base
    num = q;
    buf[i--] = r < 10 ? r + '0' : r + 'A' - 10;
  }
  while (num && i >= 0);
  
  // Add sign character?
  if (signChar != ' ' && i >= 0)
    buf[i--] = signChar;

  // Update start position if number didn't fit in minWidth
  if (i + 1 < numStart)
    numStart = i + 1; 
  
  // Fill with spaces
  while (i >= numStart)
    buf[i--] = ' ';
  
  // Print starting from numStart position
  p.write(&buf[numStart]);
}

void printCol(long num, unsigned char base/*=DEC*/, unsigned char minWidth/*=11*/, Print& p/*=Serial*/)
{
  if (base == 10 && num < 0)
  {
    // num = -num works also for LONG_MIN because in two's complement arithmetic:
    // 1. -LONG_MIN is LONG_MIN (negation is swapping the bits and adding one with wrap around)
    // 2. (unsigned long)LONG_MIN is LONG_MAX + 1
    num = -num;
    printColSign(('-'), (unsigned long)num, 10, minWidth, p);
  } 
  else
    printColSign((' '), (unsigned long)num, base, minWidth, p);
}

void printCol(unsigned long num, unsigned char base/*=DEC*/, unsigned char minWidth/*=11*/, Print& p/*=Serial*/)
{
  printColSign(' ', num, base, minWidth, p);
}

static void printColSign(char signChar, unsigned long long num, unsigned char base/*=DEC*/, unsigned char minWidth/*=11*/, Print& p/*=Serial*/)
{
  char buf[PRINTCOL_ULL_BUF_SIZE];
  
  // Checks
  if (base < 2)
    base = 2;
  else if (base > 16)
    base = 16;
  if (minWidth > (PRINTCOL_ULL_BUF_SIZE - 1))
    minWidth = (PRINTCOL_ULL_BUF_SIZE - 1);

  // Init
  int i = PRINTCOL_ULL_BUF_SIZE - 1;                   // last position (pointing to NUL)
  int numStart = PRINTCOL_ULL_BUF_SIZE - 1 - minWidth; // start position according to minWidth
  buf[i--] = '\0';                                     // NUL terminate

  // Convert
  do
  {
    unsigned long long q = num / base;
    char r = num - q * base; // faster than: r = num % base
    num = q;
    buf[i--] = r < 10 ? r + '0' : r + 'A' - 10;
  }
  while (num && i >= 0);
  
  // Add sign character?
  if (signChar != ' ' && i >= 0)
    buf[i--] = signChar;

  // Update start position if number didn't fit in minWidth
  if (i + 1 < numStart)
    numStart = i + 1; 
  
  // Fill with spaces
  while (i >= numStart)
    buf[i--] = ' ';
  
  // Print starting from numStart position
  p.write(&buf[numStart]);
}

void printCol(long long num, unsigned char base/*=DEC*/, unsigned char minWidth/*=11*/, Print& p/*=Serial*/)
{
  if (base == 10 && num < 0)
  {
    // num = -num works also for LONG_LONG_MIN because in two's complement arithmetic:
    // 1. -LONG_LONG_MIN is LONG_LONG_MIN (negation is swapping the bits and adding one with wrap around)
    // 2. (unsigned long long)LONG_LONG_MIN is LONG_LONG_MAX + 1
    num = -num;
    printColSign(('-'), (unsigned long long)num, 10, minWidth, p);
  } 
  else
    printColSign((' '), (unsigned long long)num, base, minWidth, p);
}

void printCol(unsigned long long num, unsigned char base/*=DEC*/, unsigned char minWidth/*=11*/, Print& p/*=Serial*/)
{
  printColSign(' ', num, base, minWidth, p);
}

void printCol(double num, unsigned char precision/*=2*/, unsigned char minWidth/*=11*/, Print& p/*=Serial*/)
{
  char buf[PRINTCOL_FLT_BUF_SIZE];

  // minWidth limit to avoid overflowing buf
  if (minWidth > (PRINTCOL_FLT_BUF_SIZE - 1))
    minWidth = (PRINTCOL_FLT_BUF_SIZE - 1);

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
    snprintf(buf, PRINTCOL_FLT_BUF_SIZE, fmt, num); // no more than PRINTCOL_FLT_BUF_SIZE chars (including the NUL char) are converted to buf
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
    snprintf(buf, PRINTCOL_FLT_BUF_SIZE, fmt, num); // no more than PRINTCOL_FLT_BUF_SIZE chars (including the NUL char) are converted to buf
#endif
  }
  p.write(buf);
}
