/*
  NTP timestamp to Unix epoch timestamp
  
  - Properly determine era 0 or era 1, so that NTP timestamp 
    correctly wraps in 2036.

  - We do not have the Unix timestamp 2038 rollover problem, 
    depending from the platform, time_t is either defined 
    as uint32_t or as int64_t and not as int32_t which is 
    the problematic type.
*/
#include <TimeLib.h> // by Michael Margolis, https://github.com/PaulStoffregen/Time
#include "PrintCol.h"

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

static void test()
{
  // time_t size
  Serial.print("sizeof(time_t) = "); Serial.println(sizeof(time_t));

  // SECS_IN_ERA is bigger than a 32-bit unsigned integer 
  Serial.print("SECS_IN_ERA = "); printCol(SECS_IN_ERA, DEC, 0);
  Serial.println(); Serial.println();

  // Header
  Serial.println("        NTP           Epoch             Time (UTC)");
  Serial.println("--------------------------------------------------");

  // 1970
  uint32_t ntpTimestamp = SEVENTYYEARS_SEC;
  time_t unixTimestamp = (time_t)unixEpochFromNTP(ntpTimestamp);
  printCol(ntpTimestamp); Serial.print("  -> ");
  printCol(unixTimestamp); Serial.print("   "); printTime(unixTimestamp);

  // 2010
  ntpTimestamp = SEVENTYYEARS_SEC + 40 * 366 * 86400;
  unixTimestamp = (time_t)unixEpochFromNTP(ntpTimestamp);
  printCol(ntpTimestamp); Serial.print("  -> ");
  printCol(unixTimestamp); Serial.print("   "); printTime(unixTimestamp);

  // 2030
  ntpTimestamp = SEVENTYYEARS_SEC + 60 * 366 * 86400;
  unixTimestamp = (time_t)unixEpochFromNTP(ntpTimestamp);
  printCol(ntpTimestamp); Serial.print("  -> ");
  printCol(unixTimestamp); Serial.print("   "); printTime(unixTimestamp);

  // 2036
  ntpTimestamp = UINT32_MAX;
  unixTimestamp = (time_t)unixEpochFromNTP(ntpTimestamp);
  printCol(ntpTimestamp); Serial.print("  -> ");
  printCol(unixTimestamp); Serial.print("   "); printTime(unixTimestamp);

  // 2036
  ntpTimestamp = 0;
  unixTimestamp = (time_t)unixEpochFromNTP(ntpTimestamp);
  printCol(ntpTimestamp); Serial.print("  -> ");
  printCol(unixTimestamp); Serial.print("   "); printTime(unixTimestamp);

  // 2056
  ntpTimestamp = 20 * 366 * 86400;
  unixTimestamp = (time_t)unixEpochFromNTP(ntpTimestamp);
  printCol(ntpTimestamp); Serial.print("  -> ");
  printCol(unixTimestamp); Serial.print("   "); printTime(unixTimestamp);

  // 2104
  ntpTimestamp = INT32_MAX;
  unixTimestamp = (time_t)unixEpochFromNTP(ntpTimestamp);
  printCol(ntpTimestamp); Serial.print("  -> ");
  printCol(unixTimestamp); Serial.print("   "); printTime(unixTimestamp);
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
