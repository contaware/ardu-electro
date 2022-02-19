/*  
  Line tracking KY-033 (TCRT5000) module
*/
#ifndef rc_lt_h
#define rc_lt_h

#include "RC-Config.h"

#if USE_LINE_TRACKING == 1

#include "TimerPoll.h"

const int LINE_TRACKING_MOTOR_TURN_SPEED = 170;       // 0..255
const int LINE_TRACKING_MOTOR_TURN_SLOW_SPEED = 110;  // 0..255

extern TimerPoll g_lineTrackingTimer;

extern void lineTrackingBegin();
extern void lineTracking(unsigned long elapsedTimeMs);

#endif
#endif
