/*
  NTP timestamp to Unix epoch timestamp
  
  - NTP era 0 starts on Jan 1 1900 and ends on Feb 7 2036 
    (136 years). The 32-bit unsigned NTP timestamp rollsover 
    at the end of era 0 restarting from 0, era 1 thus begins.

  - We do not have the Unix timestamp 2038 rollover problem, 
    because depending from the platform, time_t is either 
    defined as uint32_t or as int64_t and not as int32_t 
    which is the problematic type.
*/
#include <TimeLib.h>     // by Michael Margolis, https://github.com/PaulStoffregen/Time
#include "PrintCol_CW.h" // https://github.com/contaware/PrintCol_CW

#define SEVENTYYEARS_SEC_ALT    2208988800UL
#define SEVENTYYEARS_SEC        2208988800LL
#define NTP_ERA                 0LL
#define SECS_IN_ERA             (UINT32_MAX + 1LL)
static int64_t unixEpochFromNTP(uint32_t ntpTimestamp)
{
  /*
    Do not use time_t as it may be defined as uint32_t, 
    we want int64_t for all the calculations.
  */

  int64_t base = NTP_ERA;
  
  /*
    Once the actual year enters the NTP era 1 (after 2036), increment 
    the NTP_ERA macro by one and remove the following test.
    Once more than half of era 1 has elapsed (after 2104), re-introduce 
    the following test to move to era 2 if ntpTimestamp <= INT32_MAX.
  */
  if (ntpTimestamp <= (uint32_t)INT32_MAX)
    base++;

  return base * SECS_IN_ERA + (int64_t)ntpTimestamp - SEVENTYYEARS_SEC;
}

static void printTime(time_t t)
{
  int d = day(t);
  int mo = month(t);
  int y = year(t);
  int h = hour(t);
  int m = minute(t);
  int s = second(t);

  // Date
  if (d < 10) Serial.print(F("0"));
  Serial.print(d);
  Serial.print(F(" "));
  Serial.print(monthShortStr(mo));
  Serial.print(F(" "));
  Serial.print(y);
  Serial.print(F(" "));

  // Time
  if (h < 10) Serial.print(F("0"));
  Serial.print(h);
  Serial.print(F(":"));
  if (m < 10) Serial.print(F("0"));
  Serial.print(m);
  Serial.print(F(":"));
  if (s < 10) Serial.print(F("0"));
  Serial.println(s);
}

static void calc(uint32_t ntpTimestamp)
{
  // unixEpochFromNTP() works for all eras, but has to be adapted
  // each half era, see comments in unixEpochFromNTP()
  time_t unixTimestamp = (time_t)unixEpochFromNTP(ntpTimestamp);
  
  // Alternative calculation limited more or less to second half 
  // of era 0 and first half of era 1
  uint32_t unixTimestampAlt = ntpTimestamp - SEVENTYYEARS_SEC_ALT;

  // Print
  printCol(ntpTimestamp); Serial.print("  -> ");
  printCol(unixTimestamp); Serial.print("   "); 
  printCol(unixTimestampAlt); Serial.print("   "); 
  printTime(unixTimestamp);
}

static void test()
{
  // time_t size
  Serial.print("sizeof(time_t) = "); Serial.println(sizeof(time_t));

  // SECS_IN_ERA is bigger than a 32-bit unsigned integer 
  Serial.print("SECS_IN_ERA = "); printCol(SECS_IN_ERA, DEC, 0);
  Serial.println(); Serial.println();

  // Header
  Serial.println("        NTP           Epoch    Epoch(alt)              Time(UTC)");
  Serial.println("----------------------------------------------------------------");

  // 1970
  calc(SEVENTYYEARS_SEC);

  // 2010
  calc(SEVENTYYEARS_SEC + 40 * 366 * 86400);

  // 2030
  calc(SEVENTYYEARS_SEC + 60 * 366 * 86400);

  // 2036
  calc(UINT32_MAX);

  // 2036
  calc(0);

  // 2056
  calc(20 * 366 * 86400);

  // 2104
  calc(INT32_MAX);
}

void setup()
{
  // Init TimeLib cache
  second(1); second(0);

  // Init Serial (leave Serial Monitor open to see all messages)
  Serial.begin(9600); delay(5000); // wait 5s that Serial is ready

  // Test it
  test();
}

void loop()
{

}
