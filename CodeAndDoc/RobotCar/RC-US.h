/*  
  Ultrasonic sensor (HC-SR04) module
*/
#ifndef rc_us_h
#define rc_us_h

#include "RC-Config.h"

#if USE_ULTRASONIC_SENSOR == 1

#include "TimerPoll.h"

const float WALL_MIN_DISTANCE_CM = 2.0;
const float WALL_MAX_DISTANCE_CM = 400.0;

extern float g_wallDistanceCm; // Distance in cm, -1 on invalid value or when not yet calculated
extern TimerPoll g_ultrasonicTimer;

extern void ultrasonicBegin();
extern void ultrasonicMeasure(unsigned long elapsedTimeMs);

#endif
#endif
