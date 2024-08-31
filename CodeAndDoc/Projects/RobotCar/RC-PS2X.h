/*  
  PS2 controller
*/
#ifndef rc_ps2x_h
#define rc_ps2x_h

#include "RC-Config.h"

#if USE_PS2_CONTROLLER == true

#include <PS2X_lib.h>
#include "TimerPoll.h"

extern PS2X g_ps2x;
extern TimerPoll g_ps2xTimer;

extern void ps2Control();

#endif
#endif
