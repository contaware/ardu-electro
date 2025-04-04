/*
  The RTC (Real-Time Clock) embedded in the Renesas MCUs 
  (UNO R4 or Portenta C33)

  - The UNO R4 RTC is not so accurate because it uses the internal 
    LOCO oscillator, while the Portenta C33 RTC is better because it 
    uses the 32kHz crystal.

  - Please leave the C time zones and daylight saving logic disabled 
    (do not call setenv() and tzset()) because the RTC.h library is 
    not meant to use that. Moreover do not use the RTC.h library 
    daylight saving implementation, always create RTCTime with 
    SaveLight::SAVING_TIME_INACTIVE. Instead employ the Timezone 
    library.

  - The Timezone library uses the TimeLib library to handle the time.
    TimeLib always starts from the Unix time and derives the various 
    time components. TimeLib is aware of leap years and correctly 
    handles the conversion between Unix time and the time components. 
    Leap years are also correctly handled by the RTC hardware.

  - We work with UTC and only convert to local time with the Timezone 
    library when we need to display a time.
*/
#include <RTC.h>
#include <TimeLib.h>  // by Michael Margolis, https://github.com/PaulStoffregen/Time
#include <Timezone.h> // by Jack Christensen, https://github.com/JChristensen/Timezone

// TimeChangeRule(abbrev, week, dow, month, hour, offset)
// abbrev: time zone abbreviation of your choice (5 chars max)
// week:   First, Second, Third, Fourth, Last
// dow:    Sun, Mon, Tue, Wed, Thu, Fri, Sat
// month:  Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec
// hour:   hour (0-23) in local time when the rule starts
//         (the local time in effect just before the change) 
// offset: is the UTC offset in minutes for the time zone being defined

// Central Europe
// DST starts 01:00 UTC (02:00 CET) on the last Sunday of March
// DST ends 01:00 UTC (03:00 CEST) on the last Sunday of October
TimeChangeRule myDSTStart = {"CEST", Last, Sun, Mar, 2, 120};   // Daylight time = UTC + 2 hours
TimeChangeRule mySTDStart = {"CET", Last, Sun, Oct, 3, 60};     // Standard time = UTC + 1 hour

// US Eastern Time Zone
// DST starts 2:00 a.m. EST on the second Sunday of March
// DST ends 2:00 a.m. EDT on the first Sunday of November
//TimeChangeRule myDSTStart = {"EDT", Second, Sun, Mar, 2, -240}; // Daylight time = UTC - 4 hours
//TimeChangeRule mySTDStart = {"EST", First, Sun, Nov, 2, -300};  // Standard time = UTC - 5 hours

// Set DST start and end times
Timezone myTZ(myDSTStart, mySTDStart);

// Just before switching to CEST
RTCTime beforeCESTSwitch(30,                              // day: 1-31
                        Month::MARCH,                     // month: JANUARY, FEBRUARY, MARCH, APRIL, MAY, JUNE, JULY, AUGUST, SEPTEMBER, OCTOBER, NOVEMBER, DECEMBER
                        2025,                             // year: four digits
                        0,                                // hour: 0-23
                        59,                               // minutes: 0..59
                        50,                               // seconds: 0..59
                        DayOfWeek::SUNDAY,                // day of week: MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY, SATURDAY, SUNDAY
                        SaveLight::SAVING_TIME_INACTIVE); // we handle daylight saving with Timezone library

// Just before switching to CET
RTCTime beforeCETSwitch(26,                               // day: 1-31
                        Month::OCTOBER,                   // month: JANUARY, FEBRUARY, MARCH, APRIL, MAY, JUNE, JULY, AUGUST, SEPTEMBER, OCTOBER, NOVEMBER, DECEMBER
                        2025,                             // year: four digits
                        0,                                // hour: 0-23
                        59,                               // minutes: 0..59
                        50,                               // seconds: 0..59
                        DayOfWeek::SUNDAY,                // day of week: MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY, SATURDAY, SUNDAY
                        SaveLight::SAVING_TIME_INACTIVE); // we handle daylight saving with Timezone library

// Print the provided time split in its components
void printTime( int y,    // four digits year
                int mo,   // 1..12
                int d,    // 1..31
                int h,    // 0..23
                int m,    // 0..59
                int s,    // 0..59
                int wday) // day of week from 1 (Sunday) to 7 (Saturday)
{
  Serial.print(dayShortStr(wday));
  Serial.print(" ");
  Serial.print(y);
  Serial.print("-");
  if (mo < 10) Serial.print("0");
  Serial.print(mo);
  Serial.print("-");
  if (d < 10) Serial.print("0");
  Serial.print(d);
  Serial.print(" ");
  if (h < 10) Serial.print("0");
  Serial.print(h);
  Serial.print(":");
  if (m < 10) Serial.print("0");
  Serial.print(m);
  Serial.print(":");
  if (s < 10) Serial.print("0");
  Serial.print(s);
}

// Print the provided unix timestamp
void printTime(time_t t)
{
  // Use the TimeLib.h functions to split the provided time_t variable
  printTime(year(t), month(t), day(t),
            hour(t), minute(t), second(t),
            weekday(t)); // weekday(t) starts at 1 (Sunday)
}

void setup()
{
  // Init Serial (leave Serial Monitor open to see all messages)
  Serial.begin(9600); delay(5000); // wait 5s that Serial is ready

  // Begin
  if (RTC.begin())
    Serial.println("RTC has been initialized");
  else
    Serial.println("Error while initializing the RTC!");
  
  // Init the RTC using the current UTC time
  // Hint: use RTCTime(time_t t) to init from NTP.
  if (RTC.setTime(beforeCESTSwitch)) // try: beforeCESTSwitch or beforeCETSwitch
    Serial.println("RTC time has been set");
  else
    Serial.println("Could not set the RTC time!");
}

void loop()
{
  // Get the current UTC time from the RTC
  RTCTime utcRTCTime;
  RTC.getTime(utcRTCTime);
  time_t utcTimestamp = utcRTCTime.getUnixTime();
  printTime(utcTimestamp);
  Serial.println(" UTC");

  // Convert to local time for display 
  TimeChangeRule* tcr;
  time_t localTimestamp = myTZ.toLocal(utcTimestamp, &tcr);
  printTime(localTimestamp);
  Serial.print(" "); Serial.println(tcr->abbrev);

  // 1 sec delay
  Serial.println();
  delay(1000);
}
