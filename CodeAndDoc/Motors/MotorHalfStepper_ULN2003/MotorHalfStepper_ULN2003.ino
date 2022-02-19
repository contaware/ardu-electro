/*
  In half step mode the 28BYJ-48 motor increases 5.625° per half step. 
  The motor has a reduction of 1/63.68395.
  So we have 64 * 63.68395 = 4075.7728 mini-steps for a full rotation.
  That's usually approximated to 4076 mini-steps. Note that some example 
  are using the datasheet und use the wrong reduction of 1/64 which 
  gives 4096 mini-steps.
  CheapStepper library defaults to 4096 mini-steps but you can call:
  CheapStepper::set4076StepMode() to use 4076 steps or
  CheapStepper::setTotalSteps(int numSteps) to use a custom amount
*/
#include <CheapStepper.h>

/* 
  IN1 <-> PIN8
  IN2 <-> PIN9
  IN3 <-> PIN10
  IN4 <-> PIN11
*/
CheapStepper stepper(8, 9, 10, 11);

void setup()
{
  stepper.set4076StepMode();
  stepper.setRpm(18); // sets speed (10-24 rpm, hi-low torque)
}

void loop()
{
  stepper.moveDegreesCW(360);
  delay(2000);
  stepper.moveDegreesCCW(360);
  delay(2000);
}
