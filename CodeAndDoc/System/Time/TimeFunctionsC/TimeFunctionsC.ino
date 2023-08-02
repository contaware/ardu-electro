/*
  The C time functions, time zones and daylight saving
*/
#include <time.h>
#include "PrintCol.h" // used here for 64-bit serial printing

#if defined(__AVR__)
#include <util/usa_dst.h>
#include <util/eu_dst.h>
#else
/* 
The setenv() declaration is in stdlib.h, for certain versions it is guarded by:
#if __BSD_VISIBLE || __POSIX_VISIBLE >= 200112
thus we declare it also manually (for UNO R4):
*/
#include <stdlib.h>
extern "C" int setenv(const char *__string, const char *__value, int __overwrite);
#endif

void printTm(const struct tm* pTmTime)
{
  if (pTmTime)
  {
    Serial.print(pTmTime->tm_year + 1900); Serial.print('/');
  
    if (pTmTime->tm_mon < 9) Serial.print('0');
    Serial.print(pTmTime->tm_mon + 1); Serial.print('/');
    
    if (pTmTime->tm_mday < 10) Serial.print('0');
    Serial.print(pTmTime->tm_mday); Serial.print(' ');
  
    if (pTmTime->tm_hour < 10) Serial.print('0');
    Serial.print(pTmTime->tm_hour); Serial.print(':');
  
    if (pTmTime->tm_min < 10) Serial.print('0');
    Serial.print(pTmTime->tm_min); Serial.print(':');
  
    if (pTmTime->tm_sec < 10) Serial.print('0');
    Serial.print(pTmTime->tm_sec); Serial.print(' ');
    
    Serial.print("dst="); Serial.println(pTmTime->tm_isdst);
  }
}

void setup()
{
  Serial.begin(9600);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!
  delay(5000);      // for ESP32 and some other MCUs a delay() is needed, otherwise
                    // the messages generated in setup() can't be seen!

#if defined(__AVR__)
  /* 
    AVR does not support setenv() and tzset(), instead it has set_zone() and set_dst().
  */

  /* 
    Set the time zone. The parameter is given in seconds East of the prime meridian (GMT/UTC).
    If the time zone is not set, the time system will operate in UTC only.
  */
  set_zone(1 * ONE_HOUR);

  /*
    Set the DST callback function, which gets invoked for a local time to determine whether 
    it is under DST or not. If it is under DST, the function returns the number of seconds
    a clock is advanced for Daylight Saving. This will typically be ONE_HOUR.
  */
  set_dst(eu_dst); // provide usa_dst or eu_dst

  /*
    Per standard, struct tm->tm_isdst is greater than zero when Daylight Saving time is in
    effect. The AVR implementation further specifies that, when positive, the value of
    tm_isdst represents the amount of seconds to advanced during Daylight Saving time.
  */
  #define DST_ONE_HOUR      3600
#else
  /* 
    Always set a timezone, otherwise the system does not know the DST offset in seconds to
    apply and mktime() seems to not regard tm_isdst, but in reality it regards it applying
    a DST offset of 0 seconds.
  */
  setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);  // CET: https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
  tzset();                                        //      https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv

  /*
    Per standard, struct tm->tm_isdst is greater than zero when Daylight Saving time is in
    effect. Usually it is set to 1. The amount of seconds to advanced during Daylight
    Saving time is taken from the TZ variable.
  */
  #define DST_ONE_HOUR      1
#endif

  /*
    Though not specified in the standard, it is often expected that time_t is a signed
    integer representing an offset in seconds from Midnight Jan 1 1970 (Unix time). The
    AVR implementation uses an unsigned 32-bit integer offset from Midnight Jan 1 2000.
    The use of this 'epoch' helps to simplify the conversion functions, while the 32-bit
    value allows time to be properly represented until Tue Feb 7 06:28:15 2136 UTC. AVR
    defines the macros UNIX_OFFSET and NTP_OFFSET to assist in converting to and from
    Unix and NTP time stamps.
  */
  #ifndef UNIX_OFFSET
  #define UNIX_OFFSET       0
  #endif

  /*
   mktime(timeptr) reads and adjusts the passed time structure and returns a
   binary time stamp. The elements of timeptr are interpreted as representing
   Local Time. The original values of the tm_wday and tm_yday elements of the
   structure are ignored. The value specified in the tm_isdst field informs
   mktime() whether or not daylight saving time (DST) is in effect for the time
   supplied in the tm structure: a positive value means DST is in effect, zero
   means that DST is not in effect and a negative value means that mktime()
   should use the informations in the system to attempt to determine whether
   DST is in effect at the specified time.
   
   On successful completion, the values of all elements of timeptr are set to
   the appropriate range. tm_isdst is set (regardless of its initial value) to
   a positive value or to 0, respectively, to indicate whether DST is or is not
   in effect at the specified time.
  */

  // 1. DST set
  time_t timeOffset;
  long long timeOffsetUnix;
  struct tm* pTmTime;
  struct tm tmTime;
  tmTime.tm_sec = 0;             /* Seconds          [0, 59] */
  tmTime.tm_min = 0;             /* Minutes          [0, 59] */
  tmTime.tm_hour = 6;            /* Hour             [0, 23] */
  tmTime.tm_mday = 1;            /* Day of the month [1, 31] */
  tmTime.tm_mon = 6;             /* Month            [0, 11]  (January = 0) */
  tmTime.tm_year = 2023 - 1900;  /* Year minus 1900 */
  tmTime.tm_wday = 0;            /* Day of the week  [0, 6]   (Sunday = 0) */
  tmTime.tm_yday = 0;            /* Day of the year  [0, 365] (Jan/01 = 0) */
  tmTime.tm_isdst = DST_ONE_HOUR;/* Daylight saving */
  Serial.print("struct tm: "); printTm(&tmTime);
  timeOffset = mktime(&tmTime);
  timeOffsetUnix = (long long)timeOffset + UNIX_OFFSET;
  if (timeOffsetUnix == 1688184000)
  {
    Serial.print("mktime:    "); printCol(timeOffsetUnix, DEC, 0); Serial.print("<-OK     "); Serial.print(" dst="); Serial.println(tmTime.tm_isdst);
  }
  else
  {
    Serial.print("mktime:    "); printCol(timeOffsetUnix, DEC, 0); Serial.print("<-WRONG! "); Serial.print(" dst="); Serial.println(tmTime.tm_isdst);
  }
  pTmTime = localtime(&timeOffset);
  Serial.print("localtime: "); printTm(pTmTime); Serial.println();
  
  // 2. DST not set
  tmTime.tm_sec = 0;             /* Seconds          [0, 59] */
  tmTime.tm_min = 0;             /* Minutes          [0, 59] */
  tmTime.tm_hour = 5;            /* Hour             [0, 23] */
  tmTime.tm_mday = 1;            /* Day of the month [1, 31] */
  tmTime.tm_mon = 6;             /* Month            [0, 11]  (January = 0) */
  tmTime.tm_year = 2023 - 1900;  /* Year minus 1900 */
  tmTime.tm_wday = 0;            /* Day of the week  [0, 6]   (Sunday = 0) */
  tmTime.tm_yday = 0;            /* Day of the year  [0, 365] (Jan/01 = 0) */
  tmTime.tm_isdst = 0;           /* No Daylight saving */
  Serial.print("struct tm: "); printTm(&tmTime);
  timeOffset = mktime(&tmTime);
  timeOffsetUnix = (long long)timeOffset + UNIX_OFFSET;
  if (timeOffsetUnix == 1688184000)
  {
    Serial.print("mktime:    "); printCol(timeOffsetUnix, DEC, 0); Serial.print("<-OK     "); Serial.print(" dst="); Serial.println(tmTime.tm_isdst);
  }
  else
  {
    Serial.print("mktime:    "); printCol(timeOffsetUnix, DEC, 0); Serial.print("<-WRONG! "); Serial.print(" dst="); Serial.println(tmTime.tm_isdst);
  }
  pTmTime = localtime(&timeOffset);
  Serial.print("localtime: "); printTm(pTmTime); Serial.println();

  // 3. Let the system determine the DST for the given local time
  tmTime.tm_sec = 0;             /* Seconds          [0, 59] */
  tmTime.tm_min = 0;             /* Minutes          [0, 59] */
  tmTime.tm_hour = 6;            /* Hour             [0, 23] */
  tmTime.tm_mday = 1;            /* Day of the month [1, 31] */
  tmTime.tm_mon = 6;             /* Month            [0, 11]  (January = 0) */
  tmTime.tm_year = 2023 - 1900;  /* Year minus 1900 */
  tmTime.tm_wday = 0;            /* Day of the week  [0, 6]   (Sunday = 0) */
  tmTime.tm_yday = 0;            /* Day of the year  [0, 365] (Jan/01 = 0) */
  tmTime.tm_isdst = -1;          /* Calc. by the system */
  Serial.print("struct tm: "); printTm(&tmTime);
  timeOffset = mktime(&tmTime);
  timeOffsetUnix = (long long)timeOffset + UNIX_OFFSET;
  if (timeOffsetUnix == 1688184000)
  {
    Serial.print("mktime:    "); printCol(timeOffsetUnix, DEC, 0); Serial.print("<-OK     "); Serial.print(" dst="); Serial.println(tmTime.tm_isdst);
  }
  else
  {
    Serial.print("mktime:    "); printCol(timeOffsetUnix, DEC, 0); Serial.print("<-WRONG! "); Serial.print(" dst="); Serial.println(tmTime.tm_isdst);
  }
  pTmTime = localtime(&timeOffset);
  Serial.print("localtime: "); printTm(pTmTime); Serial.println();
}

void loop()
{
  
}
