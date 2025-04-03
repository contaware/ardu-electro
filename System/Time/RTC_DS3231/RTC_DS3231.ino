/*
  DS3231 precision RTC module (ZS-042) with I2C support
 
  - VDD supply is 2.3V – 5.5V (for the AT24C32 it is 2.7V - 5.5V). 
    To ensure that the time is preserved in the event of a power cut, 
    operate the module with a rechargeable LIR2032 buffer battery 
    or insert a non-rechargeable CR2032 button cell, but pay attention:

    Battery    3.3V supply                5V supply
    -------    -----------                ---------
    CR2032     Battery not affected	      Disable charging circuit
    LIR2032    Battery not affected       Disable charging circuit, or
               (charging does not work)   make sure 5V is actually 4.7V

    Disable charging circuit = remove 201 resistor above the SCL label.

  - The DS3231 is the most accurate of the commonly available chips. 
    It has an accuracy of ±3.5ppm, which translates into: 
    ±3.5ppm * 24 * 60 * 60 = ±3.5/1000000 * 86400 = ±0.3s per day

  - DS3231's I2C address is fixed at 0x68 and the module has also an 
    AT24C32 EEPROM with a default I2C address of 0x57 that can be set 
    to 0x50-0x57. The EEPROM doesn't really have anything to do with 
    the RTC, but it can be used to store any non-volatile data. 
    The SDA and SCL lines have 4.7kΩ pull-up resistors.

  - 32K	is the 32kHz oscillator output which can be used as a clock 
    reference. SQW is usually used as an interrupt output, but it can 
    also be programmed as a square wave output.

  - The RTClib library defines the DateTime class to handle the time.
    The Timezone library uses the TimeLib library to handle the time.

  - The difference between TimeLib and DateTime is the base time type, 
    TimeLib always starts from the Unix time and derives the various 
    time components, while DateTime stores the various time components 
    and derives the Unix time from them. DateTime supports dates in the 
    range from 1 Jan 2000 to 31 Dec 2099 inclusive, while TimeLib is 
    only limited by the size of time_t. Both libraries are aware of 
    leap years and correctly handle the conversion between Unix time 
    and the time components. Leap years are also correctly handled by 
    the RTC hardware, which like DateTime, stores the time by its 
    components.
    Both libraries are NOT AWARE of TZ (time zones) and DST (daylight 
    saving time). We work with UTC and only convert to local time with 
    the Timezone library when we need to display a time.
*/
#include <RTClib.h>   // by Adafruit, https://github.com/adafruit/RTClib
#include <TimeLib.h>  // by Michael Margolis, https://github.com/PaulStoffregen/Time
#include <Timezone.h> // by Jack Christensen, https://github.com/JChristensen/Timezone

// The RTC object
RTC_DS3231 rtc;

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

// Print the provided RTClib.h DateTime object
void printTime(const DateTime& dt)
{
  printTime(dt.year(), dt.month(), dt.day(),
            dt.hour(), dt.minute(), dt.second(),
            dt.dayOfTheWeek() + 1); // dt.dayOfTheWeek() starts at 0 (Sunday)
}

void setup()
{
  // Init Serial (leave Serial Monitor open to see all messages)
  Serial.begin(9600); delay(5000); // wait 5s that Serial is ready

  // Begin
  if (!rtc.begin())
  {
    Serial.println("Could not find the DS3231 RTC!");
    while (true);
  }

  // RTC lost power?
  if (rtc.lostPower())
    Serial.println("DS3231 RTC lost power, please set the correct time!");
  else
    Serial.println("DS3231 RTC still running");
  Serial.println();
  
  // Init the RTC using the current UTC time
  // Hint: use DateTime(uint32_t t) to init from NTP.
  // year (2000-2099), month (1-12), day (1-31)
  // hour (0-23), minute (0-59), second (0-59)
  rtc.adjust(DateTime(2025, 3, 30, 0, 59, 50));  // just before switching to CEST
  //rtc.adjust(DateTime(2025, 10, 26, 0, 59, 50)); // just before switching to CET
}

void loop()
{
  // Get the current UTC time from the RTC
  DateTime utcDateTime = rtc.now();
  printTime(utcDateTime);
  Serial.println(" UTC");

  // Convert to local time for display
  time_t utcTimestamp = utcDateTime.unixtime();
  TimeChangeRule* tcr;
  time_t localTimestamp = myTZ.toLocal(utcTimestamp, &tcr);
  printTime(localTimestamp);
  Serial.print(" "); Serial.println(tcr->abbrev);
  
  // Show temperature
  Serial.print(rtc.getTemperature(), 1);
  Serial.println(" °C");

  // 1 sec delay
  Serial.println();
  delay(1000);
}
