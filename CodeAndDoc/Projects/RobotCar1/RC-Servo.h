/*  
  Servo
  
  Note: on AVR boards other than the Mega, use of the library disables 
        analogWrite() (PWM) functionality on pins 9 and 10, whether or 
        not there is a Servo on those pins.
*/
#ifndef rc_servo_h
#define rc_servo_h

#include "RC-Config.h"

#if USE_SERVO == true

#include <Servo.h> // by Michael Margolis, Arduino

extern Servo g_servo;

#endif
#endif
