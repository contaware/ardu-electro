/*
  Code for a simple Buck converter circuit
 
  - The used circuit is shown in buck_schem.fzz
*/
#include "setPwmFrequency.h"

const byte POTENTIOMETER_PIN = A0;
const byte FEEDBACK_PIN = A1;
const byte PWM_PIN = 5;
int pwm = 1;

#define USE_FEEDBACK      true

void setup()
{
  pinMode(POTENTIOMETER_PIN, INPUT);
  pinMode(FEEDBACK_PIN, INPUT);
  pinMode(PWM_PIN, OUTPUT);
  setPwmFrequency(PWM_PIN, 1);
}

void loop()
{  
  float value = analogRead(POTENTIOMETER_PIN);
#if USE_FEEDBACK == true
  float feedback  = analogRead(FEEDBACK_PIN);
  if (value > feedback)
  {
    pwm = pwm + 1;
    pwm = constrain(pwm, 1, 254);
  }
  else if (value < feedback)
  {
    pwm = pwm - 1;
    pwm = constrain(pwm, 1, 254);
  }
#else
  pwm = map(value, 0, 1023, 1, 254); 
#endif
  analogWrite(PWM_PIN, pwm);
}
