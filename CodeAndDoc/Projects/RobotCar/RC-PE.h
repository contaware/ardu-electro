/*  
  Photo electric encoder (HC-020K) module
*/ 
#ifndef rc_pe_h
#define rc_pe_h

#include "RC-Config.h"

#if USE_PHOTO_ENC == true

#include "TimerPoll.h"

// The maximum RPM of the motor is 200 rpm, which with our slotted disc gives a
// frequency of 200 / 60 * 20 = 66 Hz. So the period of our square wave is 15 ms
// or 7.5 ms per logical state. A debounce time of 2-3 ms is ok because it is
// less than 7.5 ms.
const unsigned long PHOTO_ENC_DEBOUNCE_TIME_MS = 2;
const unsigned long PHOTO_ENC_CHANGES_PER_TURN = 2 * 20; // 2 * number of slots on the disc
const unsigned long PHOTO_ENC_CARWHEEL_CIRCUMFERENCE_MM = 210;

extern volatile byte g_photoEncRightCountdown;
extern volatile byte g_photoEncLeftCountdown;
extern TimerPoll g_photoEncTimer;

extern void photoEncBegin();
extern void photoEncMeasure();

#endif
#endif
