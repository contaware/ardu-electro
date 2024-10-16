/*  
  Ultrasonic sensor (HC-SR04) module
*/
#ifndef rc_us_h
#define rc_us_h

#include "RC-Config.h"

#if USE_ULTRASONIC_SENSOR == true

#include "TimerPoll.h"

const float WALL_BRAKE_DISTANCE_CM = 30.0;
const float WALL_RELEASE_DISTANCE_CM = 40.0;

extern TimerPoll g_ultrasonicTimer;

extern void ultrasonicBegin();
extern void ultrasonicMeasure();

#endif
#endif
