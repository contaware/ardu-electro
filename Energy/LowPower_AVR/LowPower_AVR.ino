/*
  AVR boards power management

  - idle()       in idle mode, the clock signal for both CPU and flash is 
                 disabled, while the ADC, timers, SPI, USART and I2C can be 
                 enabled or disabled.

  - powerDown()  this is the lowest current consumption state. Only ADC and 
                 the Brown Out Detector (BOD) can optionally be enabled.

  - If external modules and sensors are used, put them to sleep before 
    the MCU.

  - Hint for boards with native USB support:
    to upload a new sketch when in low power mode, press and hold the reset 
    button, hit the Upload button and only release the reset button after 
    you see the "Uploading..." message.

*/
#include "LowPower.h" // Low-Power by Rocket Scream Electronics

const byte BUTTON_PIN = 2;

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
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), wakeUp, FALLING); // FALLING, RISING, and CHANGE
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

  // Enter low power
  // SLEEP_15MS,SLEEP_30MS,SLEEP_60MS,SLEEP_120MS,SLEEP_250MS,SLEEP_500MS,
	// SLEEP_1S,SLEEP_2S,SLEEP_4S,SLEEP_8S,SLEEP_FOREVER
  // Note: for sleeps longer than 8 sec use a loop.
  LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF); // sleep for 4 sec or wake-up with the button
  //LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF); // infinite sleep, only wake-up with the button
}
