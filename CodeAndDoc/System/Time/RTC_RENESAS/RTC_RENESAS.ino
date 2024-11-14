/*
  The RTC (Real-Time Clock) embedded in the Renesas MCUs 
  (UNO R4 or Portenta C33)
 
  The better way to manage a RTC hardware which usually knows nothing 
  about time zones or daylight saving, is to store the time in the RTC 
  hardware as UTC; then convert it to local time before displaying it.
  
  But the RTC.h lib internally stores as local time and sets that 
  directly to the RTC hardware. As long as we leave the C time zones and 
  daylight saving logic disabled (do not call setenv() and tzset()) it 
  works because the conversion functions between unix time and internal 
  tm time will not apply offsets, so to say, what we set is what we get.

  - The RTCTime class internally stores the time in a tm structure. We 
    can access the internal tm structure with RTCTime::getTmTime() or 
    set it through RTCTime::setTM().

  - RTCTime::setUnixTime(time_t time) uses localtime() to convert 
    between the passed unix time epoch and the internal tm structure. 
    Here unexpected behavior may happen if the time zones and the 
    daylight saving logic is enabled by calling setenv() and tzset().

  - RTCTime::getUnixTime() uses mktime() to convert the internal tm 
    structure and return a unix time epoch. Here unexpected behavior may 
    happen if the time zones and the daylight saving logic is enabled by 
    calling setenv() and tzset().

  - RTC.getTime(RTCTime &t) reads a tm structure from the RTC hardware 
    and directly copies it to the tm structure of the provided RTCTime 
    object.
    Attention: RTC hardware always returns the tm_isdst set to 1, to be 
               consistent it should have returned -1 so that 
               RTCTime::getUnixTime() automatically converted from local 
               time to unix time.

  - RTC.setTime(RTCTime &t) writes the tm structure of the provided 
    RTCTime object to the RTC hardware. Note that if in RTCTime the day 
    of week is wrong, no corrections are made by the library to fix it. 
    If you want a correct day of week set automatically, then you have 
    to initialize RTCTime with a unix time.
*/
#include "RTC.h"
#include <stdlib.h>
// The setenv() declaration is in stdlib.h, but for UNO R4 it is guarded by:
// #if __BSD_VISIBLE || __POSIX_VISIBLE >= 200112
// thus we must declare it manually:
extern "C" int setenv(const char *__string, const char *__value, int __overwrite);

int prevSeconds = -1;
volatile bool alarmState = false;
volatile bool ledState = false;

void alarmCallback()
{
  alarmState = !alarmState;
}

void periodicCallback()
{
  if (ledState)
    digitalWrite(LED_BUILTIN, HIGH);
  else
    digitalWrite(LED_BUILTIN, LOW);
  ledState = !ledState;
}

void printRTCTime(RTCTime currentTime, bool showAlarmState)
{ 
  // Year
  Serial.print(currentTime.getYear());
  Serial.print("/");

  // Month
  int month = Month2int(currentTime.getMonth());
  if (month < 10) Serial.print('0');
  Serial.print(month);
  Serial.print("/");

  // Day
  int day = currentTime.getDayOfMonth();
  if (day < 10) Serial.print('0');
  Serial.print(day);
  Serial.print(" ");
  
  // Hour
  int hour = currentTime.getHour();
  if (hour < 10) Serial.print('0');
  Serial.print(hour);
  Serial.print(":");

  // Minutes
  int minutes = currentTime.getMinutes();
  if (minutes < 10) Serial.print('0');
  Serial.print(minutes);
  Serial.print(":");

  // Seconds
  int seconds = currentTime.getSeconds();
  if (seconds < 10) Serial.print('0');
  Serial.print(seconds);
  Serial.print(" ");

  // Day of week
  DayOfWeek dayOfWeek = currentTime.getDayOfWeek();
  switch (dayOfWeek)
  {
      case DayOfWeek::SUNDAY:    Serial.print("Sun"); break;
      case DayOfWeek::MONDAY:    Serial.print("Mon"); break;
      case DayOfWeek::TUESDAY:   Serial.print("Tue"); break;
      case DayOfWeek::WEDNESDAY: Serial.print("Wed"); break;
      case DayOfWeek::THURSDAY:  Serial.print("Thu"); break;
      case DayOfWeek::FRIDAY:    Serial.print("Fri"); break;
      case DayOfWeek::SATURDAY:  Serial.print("Sat"); break;
      default:                   Serial.print("Unk"); break;
  }

  // Daylight flag
  Serial.print(" dst=");
  struct tm internalTime = currentTime.getTmTime();
  Serial.print(internalTime.tm_isdst);
  
  // Unix timestamp
  Serial.print(" unix=");
  Serial.print(currentTime.getUnixTime());

  // Alarm state
  if (showAlarmState)
  {
    Serial.print(" alarm=");
    Serial.print(alarmState);
  }

  // Newline
  Serial.println();
}

void setup()
{
  Serial.begin(9600);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!

  // Init built-in LED
  pinMode(LED_BUILTIN, OUTPUT);

  // Do not set a timezone, otherwise it gets quite messed up (see explanation above)
  //setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);  // CET: https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
  //tzset();                                        //      https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv

  // Init
  if (RTC.begin())
    Serial.println("RTC has been initialized.");
  else
    Serial.println("Error while initializing the RTC.");
  
  // Set an initial time so that the RTC starts running
  RTCTime startTime(1,                                // day
                    Month::JANUARY,                   // month: JANUARY, FEBRUARY, MARCH, APRIL, MAY, JUNE, JULY, AUGUST, SEPTEMBER, OCTOBER, NOVEMBER, DECEMBER
                    2023,                             // year
                    5,                                // hour
                    0,                                // minutes
                    0,                                // seconds
                    DayOfWeek::SUNDAY,                // day of week: MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY, SATURDAY, SUNDAY
                    SaveLight::SAVING_TIME_INACTIVE); // daylight saving: SAVING_TIME_INACTIVE or SAVING_TIME_ACTIVE
  if (RTC.setTime(startTime))
    Serial.println("RTC time has been set to:");
  else
    Serial.println("Could not set the RTC time to:");
  printRTCTime(startTime, false);

  // Trigger the alarm every time the seconds are zero
  RTCTime alarmTime;
  alarmTime.setSecond(0);
  AlarmMatch matchTime;
  matchTime.addMatchSecond(); // match on the seconds
  if (RTC.setAlarmCallback(alarmCallback, alarmTime, matchTime))
    Serial.println("Periodic callback set correctly.");
  else
    Serial.println("Could not set the periodic callback.");
  
  // Periodic callback
  if (RTC.setPeriodicCallback(periodicCallback, Period::N2_TIMES_EVERY_SEC))  // ONCE_EVERY_2_SEC, ONCE_EVERY_1_SEC, N2_TIMES_EVERY_SEC, N4_TIMES_EVERY_SEC,
    Serial.println("Alarm callback set correctly.");                          // N8_TIMES_EVERY_SEC, N16_TIMES_EVERY_SEC, N32_TIMES_EVERY_SEC,
  else                                                                        // N64_TIMES_EVERY_SEC, N128_TIMES_EVERY_SEC, N256_TIMES_EVERY_SEC
    Serial.println("Could not set the alarm callback.");
}

void loop()
{
  // Get current time from RTC
  RTCTime currentTime;
  RTC.getTime(currentTime);

  // Print each second
  int seconds = currentTime.getSeconds();
  if (seconds != prevSeconds)
  {
    // Store seconds
    prevSeconds = seconds;

    // Print current RTC time with alarm state
    printRTCTime(currentTime, true);
  }

  // Do not poll RTC too often
  delay(100);
}
