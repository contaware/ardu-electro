/*
  Drive an unipolar Stepper motor with the ULN2003A chip/module in full step mode

  - In full step mode the 28BYJ-48 motor increases 11.25° (360/32) per
    step. The datasheet and some motor variants have a nice gear reduction
    of 1/64 which gives 2048 steps for a full rotation.
    Attention: many motor variants do not have the 1/64 gear reduction,
    they have a gear reduction of 1/63.68395. So we have 
    32 * 63.68395 = 2037.8864 steps for a full rotation... that's not good
    for an open-loop system like a stepper motor.

  - The ULN2003A contains 7 NPN darlington transistors which can drive 
    500mA each. There are clamp diodes to protect the transistors from 
    the reverse voltage peaks that occur across the motor coils when 
    switching the coils off. The bases of the transistors can be driven
    by 5V or 3.3V logic levels. The VCC can be a maximum of 50V.

  - Connect the ULN2003A chip to the Arduino and the motor like:
                    -----u-----
       BASE1 (IN1) |1        16| COLLECTOR1 (A)
       BASE2 (IN2) |2        15| COLLECTOR2 (B)
       BASE3 (IN3) |3        14| COLLECTOR3 (C)
       BASE4 (IN4) |4        13| COLLECTOR4 (D)
             BASE5 |5        12| COLLECTOR5
             BASE6 |6        11| COLLECTOR6
             BASE7 |7        10| COLLECTOR7
    EMITTERS (GND) |8         9| DIODES CATHODES (VCC MOTOR)
                    -----------

*/
#include <Stepper.h>
#define STEPS     2048

/* 
  Looking at the module we have:
  Arduino PIN11 <-> IN1 -> A (blue)
  Arduino PIN10 <-> IN2 -> B (pink)
  Arduino PIN9  <-> IN3 -> C (yellow)
  Arduino PIN8  <-> IN4 -> D (orange)

           coil1     coil2
           /   \     /   \
   step  arg1 arg2 arg3 arg4
   ----  ---- ---- ---- ----
   1        1    0    1    0
   2        0    1    1    0
   3        0    1    0    1
   4        1    0    0    1

   Note: when starting the sketch the first step is not output,
         with the first stepper.step(1) call we will be at step 2.
*/
Stepper stepper(STEPS,  // number of steps in one revolution
                11,     // arg1 (coil1)
                9,      // arg2 (coil1)
                8,      // arg3 (coil2)
                10);    // arg4 (coil2)

                
  
void setup()
{
  stepper.setSpeed(18);   // rpm, from 1..STEPS
}
 
void loop()
{
  int steps = 1;          // positive for CW and negative for CCW
  stepper.step(steps);
  delay(500);
}
