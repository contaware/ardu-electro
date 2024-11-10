/*  
  DC Motor (L298)
*/
#ifndef rc_motor_h
#define rc_motor_h

#include "RC-Config.h"

extern int g_motorSpeed;                          // 0..255
extern int g_motorLeftState;                      // 0: brake, 1: forward, -1: backward
extern int g_motorRightState;                     // 0: brake, 1: forward, -1: backward
extern bool g_forceBrake;                         // true: force brake, false: normal operation

extern void motorBegin();
extern void motorSpeed(int motorSpeed);           // 0..255
extern void motorLeftState(int motorState);       // 0: brake, 1: forward, -1: backward
extern void motorRightState(int motorState);      // 0: brake, 1: forward, -1: backward

#endif
