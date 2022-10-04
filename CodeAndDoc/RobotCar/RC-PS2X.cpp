#include "RC-PS2X.h"

#if USE_PS2_CONTROLLER == true

#include "RC-Motor.h"

PS2X g_ps2x;
TimerPoll g_ps2xTimer;

/* 
  // To find out the soft start function I used
  for (int i = 0 ; i < 128 ; i++)
  {
    char buf[10];
    sprintf(buf, "%03d ", i); 
    DPRINT(buf);
  }
  DPRINTLN();
  for (int i = 0 ; i < 128 ; i++)
  {
    int val = pow(1.0389, i);
    char buf[10];
    sprintf(buf, "%03d ", val); 
    DPRINT(buf);
  }
  DPRINTLN();
*/
void ps2Control(unsigned long elapsedTimeMs)
{
  // Read Gamepad to get new values (call this at least once a second)
  g_ps2x.read_gamepad();
  
  // Cycle mode
  if (g_ps2x.ButtonPressed(PSB_PAD_UP))
    setMode(g_mode + 1);
  else if (g_ps2x.ButtonPressed(PSB_PAD_DOWN))
    setMode(g_mode - 1);
    
  // Control Motors?
  if (g_mode == 3)
  {
    // Brake
    if (g_ps2x.Button(PSB_L1) || g_ps2x.Button(PSB_R1)  ||
        g_ps2x.Button(PSB_L2) || g_ps2x.Button(PSB_R2)  ||
        g_ps2x.Button(PSB_L3) || g_ps2x.Button(PSB_R3))
    {
      motorLeftState(0);
      motorRightState(0);
    }
    else
    {
      // Left turn
      int value = g_ps2x.Analog(PSS_RX);
      if (value < 127) // 126 -> 0
      {
        value = 127 - value;                  // 1 .. 127
        value = pow(1.0389, value);           // make it more soft start
        value = value * 175 / 127 + 80;       // 81 .. 255
        motorSpeed(value);
        motorLeftState(-1);
        motorRightState(1);
      }
      // Right turn
      else if (value > 128) // 129 -> 255
      {
        value = value - 128;                  // 1 .. 127
        value = pow(1.0389, value);           // make it more soft start
        value = value * 175 / 127 + 80;       // 81 .. 255
        motorSpeed(value);
        motorLeftState(1);
        motorRightState(-1);
      }
      else
      {
        // Forward
        value = g_ps2x.Analog(PSS_LY); 
        if (value < 127) // 126 -> 0
        {
          value = 127 - value;              // 1 .. 127
          value = pow(1.0389, value);       // make it more soft start
          value = value * 175 / 127 + 80;   // 81 .. 255
          motorSpeed(value);
          motorLeftState(1);
          motorRightState(1);
        }
        // Backward
        else if (value > 128) // 129 -> 255
        {
          value = value - 128;                // 1 .. 127
          value = pow(1.0389, value);         // make it more soft start
          value = value * 175 / 127 + 80;     // 81 .. 255
          motorSpeed(value);
          motorLeftState(-1);
          motorRightState(-1);
        }
        // Stop
        else
          motorSpeed(0);
      }
    }
  }
}

#endif
