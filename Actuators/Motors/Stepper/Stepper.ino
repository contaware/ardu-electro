/*
  Drive a stepper motor in full step mode

  A. Unipolar 28BYJ-48 stepper with ULN2003A driver
  
  - In full step mode the 28BYJ-48 motor increases 11.25° (360/32) per
    step. Some motor variants have a nice gear reduction of 1/64 which 
    gives 2048 steps for a full rotation.
    Attention: many motor variants do not have the 1/64 gear reduction,
    they have a gear reduction of 1/63.68395. 
    That gives 32 * 63.68395 = 2037.8864 steps for a full rotation...

  - The unipolar steppers can invert the coil current without a H-bridge 
    because they have central taps on the coils. The ULN2003A contains
    7 NPN Darlington transistors, each one can drive 500mA. There are 
    clamp diodes to protect the transistors from the reverse voltage 
    peaks that occur across the motor coils when switching the coils off.
    The bases of the transistors can be driven by 5V or 3.3V logic levels.
    The ULN2003A supports a VCC up to 50V.

                                   -----u-----
    Arduino PIN11 <-> BASE1 (IN1) |1        16| COLLECTOR1 <-> A=blue
    Arduino PIN10 <-> BASE2 (IN2) |2        15| COLLECTOR2 <-> B=pink
    Arduino PIN9  <-> BASE3 (IN3) |3        14| COLLECTOR3 <-> C=yellow
    Arduino PIN8  <-> BASE4 (IN4) |4        13| COLLECTOR4 <-> D=orange
                            BASE5 |5        12| COLLECTOR5
                            BASE6 |6        11| COLLECTOR6
                            BASE7 |7        10| COLLECTOR7
                 GND <-> EMITTERS |8         9| DIODES CATHODES <-> VCC MOTOR
                                   -----------               (motor red wire)
                          ULN2003A (500mA per Darlington)


  B. Bipolar stepper with L293D/SN754410 drivers
  
  - Bipolar steppers are driven by H-bridges, which are necessary to invert
    the coil current directions.
    
                            -----u-----
     set HIGH <-> ENABLE12 |1        16| VCC signal (4.5V - 5.5V)
      Arduino PIN11 <-> 1A |2        15| 4A <-> Arduino PIN10
             COIL1+ <-> 1Y |3        14| 4Y <-> COIL2+
                       GND |4        13| GND
                       GND |5        12| GND
             COIL1- <-> 2Y |6        11| 3Y <-> COIL2-
       Arduino PIN9 <-> 2A |7        10| 3A <-> Arduino PIN8
    VCC motor (4.5V - 36V) |8         9| ENABLE34 <-> set HIGH
                            -----------
                     L293D (600mA per channel)
                     SN754410 (1A per channel)
*/
#include <Stepper.h> // by Arduino
#define STEPS  2048  // 2048 for 28BYJ-48 and 200 for common bipolar steppers

/*
           coil1     coil2
           /   \     /   \
   step  arg1 arg2 arg3 arg4
   ----  ---- ---- ---- ----
   1        1    0    1    0
   2        0    1    1    0
   3        0    1    0    1
   4        1    0    0    1

   Note: when starting the sketch, step 1 is not output, and after the 
         first stepper.step(1) call, we are at step 2.
*/
Stepper stepper(STEPS,  // number of steps in one revolution
                11,     // arg1
                9,      // arg2
                8,      // arg3
                10);    // arg4

void setup()
{
  stepper.setSpeed(12); // rpm, from 1..STEPS
}
 
void loop()
{
  int steps = 1;        // positive for CW and negative for CCW
  stepper.step(steps);
  delay(500);
}
