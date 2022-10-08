/*  
  DC Motor (L298)
*/
#ifndef rc_motor_h
#define rc_motor_h

#include "RC-Config.h"

extern int g_motorSpeed;                          // 0..255
extern int g_motorLeftState;                      // 0: brake, 1: forward, -1: backward
extern int g_motorRightState;                     // 0: brake, 1: forward, -1: backward

extern void motorBegin();
extern void motorSpeed(int motorSpeed);           // 0..255
extern void motorLeftState(int motorState);       // 0: brake, 1: forward, -1: backward
extern void motorRightState(int motorState);      // 0: brake, 1: forward, -1: backward

#if USE_MOTOR_AUTO == true
const float MOTOR_ALERT_WALL_DISTANCE_CM = 28.0;  // WALL_MIN_DISTANCE_CM..WALL_MAX_DISTANCE_CM
const long MOTOR_TURN_SPEED = 160;                // 0..255
const long MOTOR_MOVE_SPEED = 120;                // 0..255
const long MOTOR_TURN_CHANGES_180 = 56;           // 1..255 (this is an experimental value with the given speed)
const long MOTOR_MOVE_CHANGES_1M = 174;           // 1..255 (this is an experimental value with the given speed, calc. would be 1000UL * PHOTO_ENC_CHANGES_PER_TURN / PHOTO_ENC_CARWHEEL_CIRCUMFERENCE_MM)
extern void motorAutoProcess();                   // auto move around
extern bool motorAutoDone();                      // auto movement done?
extern void motorAutoReset();                     // resets the auto movement, usually a motors stop or a brake is also issued with this function
extern void motorAutoMove(int nPhotoEncChanges);  // -255..255 (positive: forward, negative: backward)
extern void motorAutoTurn(int nPhotoEncChanges);  // -255..255 (positive: CW turn, negative: CCW turn)
#endif

#endif
