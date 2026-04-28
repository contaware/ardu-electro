/*  
  Ultrasonic sensor (HC-SR04) module
*/
#ifndef rc_us_h
#define rc_us_h

#include "RC-Config.h"

#if USE_ULTRASONIC_SENSOR == true

#include "TimerPoll_CW.h"

extern TimerPoll_CW g_ultrasonicTimer;

extern void ultrasonicBegin();
extern void ultrasonicMeasure();

#endif
#endif
