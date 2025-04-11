/*
  SAMD boards power management (not working for SAMD51)

  - idle()       only the CPU is stopped, but it wakes-up after a few ms 
                 because of the SysTick interrupt, thus it is of little use:
                 https://github.com/arduino-libraries/ArduinoLowPower/issues/39
                 
  - sleep()      for SAMD those two functions are the same. RTC or interrupt 
    deepSleep()  capable pins can wake-up, and with attachAdcInterrupt it's also 
                 possible to wake-up when an analog pin enters a specified range. 

  - If external modules and sensors are used, put them to sleep before 
    the MCU.
  
  - Hint: to upload a new sketch when in low power mode, enter the bootloader 
          with a double-tap reset.

  - https://docs.arduino.cc/learn/electronics/low-power
*/
#include <ArduinoLowPower.h>

// Pins used to trigger wake-ups
const byte BUTTON_PIN = 8;
const byte VOLTAGE_SENSE_PIN = A0;

void wakeUp()
{
  // Handler for the pin interrupt.
}

void setup()
{               
  // Pin modes
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Attach interrupt
  LowPower.attachInterruptWakeup(BUTTON_PIN, wakeUp, FALLING); // FALLING, RISING, and CHANGE
}

void loop()
{
  // Flash LED
  for (int i = 0 ; i < 6 ; i++)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
  }

  // This should be called immediately before LowPower.sleep() 
  // because it reconfigures the ADC internally.
  // Range types: ADC_INT_BETWEEN, ADC_INT_OUTSIDE, ADC_INT_ABOVE_MIN, ADC_INT_BELOW_MAX
  LowPower.attachAdcInterrupt(VOLTAGE_SENSE_PIN, wakeUp, ADC_INT_BETWEEN, 511, 513);

  // Enter low power
  //LowPower.sleep(4000); // sleep for 4 sec or wake-up with the button or with the ADC
  LowPower.sleep(); // infinite sleep, only wake-up with the button or with the ADC

  // This should be called immediately after LowPower.sleep() 
  // because it restores the ADC configuration after waking up.
  LowPower.detachAdcInterrupt();
}
