#include "RC-Motor.h"
#include "RC-PE.h"
#include "RC-US.h"
#include "RC-Servo.h"
#include <util/atomic.h>

int g_motorSpeed = 0;       // 0..255
int g_motorLeftState = 0;   // 0: brake, 1: forward, -1: backward
int g_motorRightState = 0;  // 0: brake, 1: forward, -1: backward

void motorBegin()
{
  pinMode(MOTOR_INX_LEFT_PIN, OUTPUT);
  pinMode(MOTOR_INY_LEFT_PIN, OUTPUT);
  pinMode(MOTOR_INX_RIGHT_PIN, OUTPUT);
  pinMode(MOTOR_INY_RIGHT_PIN, OUTPUT);
  pinMode(MOTOR_EN_LEFT_PIN, OUTPUT);
  pinMode(MOTOR_EN_RIGHT_PIN, OUTPUT);
  motorSpeed(0);
  motorLeftState(0);
  motorRightState(0);
}

void motorSpeed(int motorSpeed) // 0..255
{
  g_motorSpeed = motorSpeed;
  analogWrite(MOTOR_EN_LEFT_PIN, motorSpeed);
  analogWrite(MOTOR_EN_RIGHT_PIN, motorSpeed);
}

void motorLeftState(int motorState) // 0: brake, 1: forward, -1: backward
{
  g_motorLeftState = motorState;
  if (motorState >= 1)
  {
    digitalWrite(MOTOR_INX_LEFT_PIN, HIGH);
    digitalWrite(MOTOR_INY_LEFT_PIN, LOW);
  }
  else if (motorState <= -1)
  {
    digitalWrite(MOTOR_INX_LEFT_PIN, LOW);
    digitalWrite(MOTOR_INY_LEFT_PIN, HIGH);
  }
  else
  {
    digitalWrite(MOTOR_INX_LEFT_PIN, HIGH);
    digitalWrite(MOTOR_INY_LEFT_PIN, HIGH);
  }
}

void motorRightState(int motorState) // 0: brake, 1: forward, -1: backward
{
  g_motorRightState = motorState;
  if (motorState >= 1)
  {
    digitalWrite(MOTOR_INX_RIGHT_PIN, LOW);
    digitalWrite(MOTOR_INY_RIGHT_PIN, HIGH);
  }
  else if (motorState <= -1)
  {
    digitalWrite(MOTOR_INX_RIGHT_PIN, HIGH);
    digitalWrite(MOTOR_INY_RIGHT_PIN, LOW);
  }
  else
  {
    digitalWrite(MOTOR_INX_RIGHT_PIN, HIGH);
    digitalWrite(MOTOR_INY_RIGHT_PIN, HIGH);
  }
}


#if USE_MOTOR_AUTO == 1

bool motorAutoForceForward1M = true;
int motorAutoScanPos = -1;  // -1: no scan, 0=0°, 1=30°, 2=60°, 3=90°, 4=120°, 5=150°, 6=180°
unsigned long motorAutoScanStartMs;
float motorAutoMinDistanceCm;
int motorAutoMinDistancePos;

void motorAutoProcess()
{
  if (motorAutoScanPos >= 0)
  {
    if (millis() - motorAutoScanStartMs >= (motorAutoScanPos == 0 ? 1000 : 500))
    {
      // Update min distance
      if (g_wallDistanceCm >= 0 && g_wallDistanceCm < motorAutoMinDistanceCm)
      {
        motorAutoMinDistancePos = motorAutoScanPos;
        motorAutoMinDistanceCm = g_wallDistanceCm;
      } 

      // Next scan position
      if (motorAutoScanPos < 6)
      {
        motorAutoScanStartMs = millis();
        ++motorAutoScanPos;
        g_servo.write(30 * motorAutoScanPos);
      }
      // Done
      else
      {
        // Turn away
        if (motorAutoMinDistancePos == 0)                     // wall @ 03 o'clock
          motorAutoTurn(-90 * MOTOR_TURN_CHANGES_180 / 180);  // -> turn 90° CCW
        else if (motorAutoMinDistancePos == 1)                // wall @ 02 o'clock
          motorAutoTurn(-120 * MOTOR_TURN_CHANGES_180 / 180); // -> turn 120° CCW
        else if (motorAutoMinDistancePos == 2)                // wall @ 01 o'clock
          motorAutoTurn(-150 * MOTOR_TURN_CHANGES_180 / 180); // -> turn 150° CCW
        else if (motorAutoMinDistancePos == 3)                // wall @ 12 o'clock
          motorAutoTurn(180 * MOTOR_TURN_CHANGES_180 / 180);  // -> turn 180° CW
        else if (motorAutoMinDistancePos == 4)                // wall @ 11 o'clock
          motorAutoTurn(150 * MOTOR_TURN_CHANGES_180 / 180);  // -> turn 150° CW
        else if (motorAutoMinDistancePos == 5)                // wall @ 10 o'clock
          motorAutoTurn(120 * MOTOR_TURN_CHANGES_180 / 180);  // -> turn 120° CW
        else                                                  // wall @ 09 o'clock
          motorAutoTurn(90 * MOTOR_TURN_CHANGES_180 / 180);   // -> turn 90° CW

        // After turn away forward at least 1M
        motorAutoForceForward1M = true;
        
        // Reset
        motorAutoScanPos = -1;
        g_servo.write(90);
      }
    }
  }
  else
  {
    // If obstacle detected (while moving forward)
    bool blockForward = (g_wallDistanceCm != -1.0 && g_wallDistanceCm < MOTOR_ALERT_WALL_DISTANCE_CM);
    if (blockForward && g_motorLeftState > 0 && g_motorRightState > 0)
    {
      // Brake
      motorLeftState(0);
      motorRightState(0);

      // Reset auto movement
      motorAutoReset();
      
      // Init distances scan
      motorAutoScanStartMs = millis();
      motorAutoScanPos = 0;
      motorAutoMinDistanceCm = WALL_MAX_DISTANCE_CM;
      motorAutoMinDistancePos = 3; // init to 3 in case that all distances are -1
      g_servo.write(0); // turn servo completely right
    }
    // Do next movement
    else if (motorAutoDone())
    {
      if (motorAutoForceForward1M)
      {
        motorAutoMove(MOTOR_MOVE_CHANGES_1M); // forward 1M
        motorAutoForceForward1M = false;
      }
      else
      {
        int randomNum = random(1, 8); // 1..7
        if (randomNum <= 5)
          motorAutoMove(randomNum * MOTOR_MOVE_CHANGES_1M / 5); // forward 0.2M..1M
        else if (randomNum == 6)
          motorAutoTurn(-30 * MOTOR_TURN_CHANGES_180 / 180);    // turn 30° CCW
        else
          motorAutoTurn(30 * MOTOR_TURN_CHANGES_180 / 180);     // turn 30° CW
      }
    }
  }
}

bool motorAutoDone()
{
  return g_photoEncLeftCountdown == 0 && g_photoEncRightCountdown == 0;
}

void motorAutoReset()
{
  g_photoEncLeftCountdown = 0;
  g_photoEncRightCountdown = 0;
}

void motorAutoMove(int nPhotoEncChanges) // -255..255 (positive: forward, negative: backward)
{
  byte countdown = (byte)constrain(nPhotoEncChanges >= 0 ? nPhotoEncChanges : -nPhotoEncChanges, 0, 255);
  if (nPhotoEncChanges >= 0)
  {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
      motorSpeed(MOTOR_AUTO_SPEED);
      motorLeftState(1);
      motorRightState(1);
      g_photoEncLeftCountdown = g_photoEncRightCountdown = countdown;
    }
  }
  else
  {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
      motorSpeed(MOTOR_AUTO_SPEED);
      motorLeftState(-1);
      motorRightState(-1);
      g_photoEncLeftCountdown = g_photoEncRightCountdown = countdown;
    }
  }

  return true;
}

void motorAutoTurn(int nPhotoEncChanges) // -255..255 (positive: CW turn, negative: CCW turn)
{
  byte countdown = (byte)constrain(nPhotoEncChanges >= 0 ? nPhotoEncChanges : -nPhotoEncChanges, 0, 255);
  if (nPhotoEncChanges >= 0)
  {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
      motorSpeed(MOTOR_AUTO_SPEED);
      motorLeftState(1);
      motorRightState(-1);
      g_photoEncLeftCountdown = g_photoEncRightCountdown = countdown;
    }
  }
  else
  {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
      motorSpeed(MOTOR_AUTO_SPEED);
      motorLeftState(-1);
      motorRightState(1);
      g_photoEncLeftCountdown = g_photoEncRightCountdown = countdown;
    }
  }
}

#endif
