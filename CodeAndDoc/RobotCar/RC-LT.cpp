#include "RC-LT.h"

#if USE_LINE_TRACKING == true

#include "RC-Motor.h"

TimerPoll g_lineTrackingTimer;

void lineTrackingBegin()
{
  pinMode(LINE_TRACKING_LEFT_PIN, INPUT);
  pinMode(LINE_TRACKING_MIDDLE_PIN, INPUT);
  pinMode(LINE_TRACKING_RIGHT_PIN, INPUT);
  g_lineTrackingTimer.begin(8, lineTracking);
}

void lineTracking(unsigned long elapsedTimeMs)
{
  /*
    Current State
    
    black right:  bit0
    black middle: bit1
    black left:   bit2
  
    000: no black
    001: black right only 
    010: black middle only 
    011: black middle + right
    100: black left only
    101: black left + right (unusual)
    110: black left + middle
    111: black all
  */
  byte currentState = 0;
  if (digitalRead(LINE_TRACKING_RIGHT_PIN) == LOW)
    currentState |= 0b00000001;
  if (digitalRead(LINE_TRACKING_MIDDLE_PIN) == LOW)
    currentState |= 0b00000010;
  if (digitalRead(LINE_TRACKING_LEFT_PIN) == LOW)
    currentState |= 0b00000100;

  // Correction?
  bool directionCorrected = false;
  if (currentState == 0b00000001)       // black right
  {
    // Right turn
    motorSpeed(LINE_TRACKING_MOTOR_TURN_SPEED);
    motorLeftState(1);
    motorRightState(-1);
    directionCorrected = true;
  }   
  else if (currentState == 0b00000100)  // black left
  { 
    // Left turn
    motorSpeed(LINE_TRACKING_MOTOR_TURN_SPEED);
    motorLeftState(-1);
    motorRightState(1);
    directionCorrected = true;
  }
  else if (currentState == 0b00000011)  // black middle + right
  {
    // If doing right turn -> slow speed
    if (g_motorLeftState > 0 && g_motorRightState < 0)
    {
      motorSpeed(LINE_TRACKING_MOTOR_TURN_SLOW_SPEED);
      directionCorrected = true;
    }
  }
  else if (currentState == 0b00000110)  // black left + middle
  {
    // If doing left turn -> slow speed
    if (g_motorLeftState < 0 && g_motorRightState > 0)
    {
      motorSpeed(LINE_TRACKING_MOTOR_TURN_SLOW_SPEED);
      directionCorrected = true;
    }
  }

  // Forward?
  if (!directionCorrected)
  {
    motorSpeed(LINE_TRACKING_MOTOR_FORWARD_SPEED);
    motorLeftState(1);
    motorRightState(1);
  }
}

#endif
