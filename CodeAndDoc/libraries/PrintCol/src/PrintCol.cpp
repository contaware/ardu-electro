/*
  Serial print with column width adjustment for tabular output
*/

#include "PrintCol.h"

/*
DO NOT lower that define under 21 because dtostrf() and 
dtostre() used in printCol(double num, ...) do not have 
buffer size checks like snprintf() used in the integer
printCol(...) functions!
*/
#define PRINTCOL_BUF_SIZE                 21

/*
This size must be set according the number of digits of
the (PRINTCOL_BUF_SIZE - 1) number:
- if 1 digit  set it to 5
- if 2 digits set it to 6
- if 3 digits set it to 7
*/
#define PRINTCOL_FORMAT_BUF_SIZE          6

void printCol(char num, unsigned char base/*=DEC*/, unsigned char minWidth/*=11*/, Print& p/*=Serial*/)
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
  char form[PRINTCOL_FORMAT_BUF_SIZE];
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
    sprintf(form, "%%%ulX", minWidth);
  else if (base == OCT)
    sprintf(form, "%%%ulo", minWidth);
  else // DEC
    sprintf(form, "%%%uld", minWidth);

  // No more than PRINTCOL_BUF_SIZE characters (including
  // the trailing NUL character) are converted to buf
  snprintf(buf, PRINTCOL_BUF_SIZE, form, num);
  p.write(buf);
}

void printCol(unsigned long num, unsigned char base/*=DEC*/, unsigned char minWidth/*=11*/, Print& p/*=Serial*/)
{
  char form[PRINTCOL_FORMAT_BUF_SIZE];
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
    sprintf(form, "%%%ulX", minWidth);
  else if (base == OCT)
    sprintf(form, "%%%ulo", minWidth);
  else // DEC
    sprintf(form, "%%%ulu", minWidth);

  // No more than PRINTCOL_BUF_SIZE characters (including
  // the trailing NUL character) are converted to buf
  snprintf(buf, PRINTCOL_BUF_SIZE, form, num);
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
    // minWidth is the sum of all chars in "[-]d.ppp", while precision is the sum of all p
    dtostrf(num, minWidth, precision, buf);
    p.write(buf);
  }
  else
  {
    // minWidth is the sum of all chars in "[-]d.pppe±dd", while precision is the sum of all p
    dtostre(num, buf, precision, 0); // last parameter could be set to DTOSTR_UPPERCASE, 
                                     // but let's leave the default: e, nan, inf

    // Align to the right
    int srcPos = strlen(buf);        // starts by pointing to the NUL termination
    int dstPos = minWidth;           // starts by pointing to the last position
    if (srcPos < dstPos)             // move the string up?
    {
      while (srcPos >= 0)            // continue to copy until reaching the first char
        buf[dstPos--] = buf[srcPos--];
      while (dstPos >= 0)            // fill with spaces until reaching the first position
        buf[dstPos--] = ' ';
    }
    p.write(buf);
  }
}
