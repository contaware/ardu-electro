/*
  MKR battery and power management
 
  - MKR boards must be supplied by USB, or by a Vin of 5V (not more and not less)
    or by a single cell 3.7V Li-Po or Li-Ion of 1000mAh or more (the charging
    current is given and fixed, so never use batteries with less capacity).

  - Most batteries you can purchase on-line have the polarity reversed in comparison
    to the plug on the Arduino board. In-built battery protection circuit stops the
    battery from working, just invert the battery cables at the JST PH2 connector.

  - Standard so called Li-Po protection circuits integrated into many packs do 
    cutoff at around 2.4V, better is to cut the battery power at 3.0-3.2V with a
    MOSFET circuit controlled by your Arduino.

  - idle():      only the CPU is stopped. This mode allows power optimization with
                 the fastest wake-up time. Note that it wakes-up after 1 ms because
                 of the SysTick interrupt, thus idle(millis) makes not much sense:
                 https://github.com/arduino-libraries/ArduinoLowPower/issues/39
                 
  - sleep():     this mode allows power optimization with a slower wakeup time. 
                 Only chosen peripherals are on (on MKR Zero powered from battery 
                 cosumption is 0.7mA).
                 
  - deepSleep(): this mode allows power optimization with the slowest wake-up time.
                 All but the RTC peripherals are stopped. The CPU can be wakeup only
                 using RTC or wakeup on interrupt capable pins (on MKR Zero powered
                 from battery cosumption is 0.7mA, I could not measure a difference
                 with the sleep() mode).

  - If external modules and sensors are used, put them to sleep before the MCU.
  
  - https://docs.arduino.cc/tutorials/mkr-wifi-1010/powering-with-batteries
    https://docs.arduino.cc/learn/electronics/low-power
*/
#include <ArduinoLowPower.h> // low power features of SAMD and nRF52

const byte BUTTON_PIN = 8;
volatile int repetitions = 1;
void repetitionsIncrease()
{
  ++repetitions;
}
void repetitionsReset()
{
  repetitions = 1;
}

void setup()
{               
  // Pin modes
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Attach interrupts
  LowPower.attachInterruptWakeup(BUTTON_PIN, repetitionsReset, CHANGE); // FALLING, RISING, and CHANGE
  LowPower.attachInterruptWakeup(RTC_ALARM_WAKEUP, repetitionsIncrease, CHANGE); // FALLING, RISING, and CHANGE
}

void loop()
{
  // "Display" battery voltage with flashes 
  // In front of the ADC_BATTERY analog input there is a voltage
  // divider consisting of two resistors 1.2MΩ and 330kΩ
  // adcVolt = battVolt * 1.2 / (1.2 + 0.33)
  float battVolt = analogRead(ADC_BATTERY) * 3.3 / 1023.0 / 1.2 * (1.2 + 0.33);
  int digit1 = (int)battVolt;
  int digit2 = (int)(10.0 * (battVolt - digit1)); 
  for (int i = 0 ; i < digit1 ; i++)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }
  delay(1000);
  for (int i = 0 ; i < digit2 ; i++)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(300);
    digitalWrite(LED_BUILTIN, LOW);
    delay(300);
  }
  LowPower.sleep(5000); // sleep for 5 sec or wake-up with the button (LowPower.sleep() for infinite sleep)
  
  // "Display" repetitions (max 10)
  int localRepetitions = repetitions;
  if (localRepetitions > 10)
    localRepetitions = 10;
  for (int i = 0; i < repetitions; i++)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(300);
    digitalWrite(LED_BUILTIN, LOW);
    delay(300);
  }
  LowPower.sleep(5000); // sleep for 5 sec or wake-up with the button (LowPower.sleep() for infinite sleep)
}
