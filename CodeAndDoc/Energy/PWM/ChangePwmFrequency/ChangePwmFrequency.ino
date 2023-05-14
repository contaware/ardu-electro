/*
  Code for setting the PWM frequency and regulating the duty-cycle
  through a potentiometer
 
  - The potentiometer pin is used to read the externally set
    voltage which gets converted into the duty-cycle.
  
  - The PWM frequency is set via the setPwmFrequency() function.
*/
#include "setPwmFrequency.h"

const byte POTENTIOMETER_PIN = A0;
const byte PWM_PIN = 3;

void setup()
{
  pinMode(POTENTIOMETER_PIN, INPUT);
  pinMode(PWM_PIN, OUTPUT);
  setPwmFrequency(PWM_PIN, 256);
}

void loop()
{  
  int value = analogRead(POTENTIOMETER_PIN);
  int pwm = map(value, 0, 1023, 0, 255);
  analogWrite(PWM_PIN, pwm);
}
