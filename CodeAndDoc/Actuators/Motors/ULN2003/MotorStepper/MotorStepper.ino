/*
  In full step mode the 28BYJ-48 motor increases 11.25° per step. 
  The motor has a reduction of 1/63.68395.
  So we have 32 * 63.68395 = 2037.8864 steps for a full rotation.
  That's usually approximated to 2038 steps. Note that some example 
  are using the datasheet und use the wrong reduction of 1/64 which 
  gives 2048 steps.
*/
#include <Stepper.h>
#define STEPS 2038

/* 
  IN1 <-> PIN8
  IN2 <-> PIN9
  IN3 <-> PIN10
  IN4 <-> PIN11
*/
Stepper stepper(STEPS, 8, 10, 9, 11);
  
void setup()
{
  stepper.setSpeed(18); // rpm
}
 
void loop()
{
  stepper.step(STEPS);
  delay(2000);
  stepper.step(-STEPS);
  delay(2000);
}
