#include "RC-Motor.h"
#include "RC-LCD.h"

int g_motorSpeed = 0;       // 0..255
int g_motorLeftState = 0;   // 0: brake, 1: forward, -1: backward
int g_motorRightState = 0;  // 0: brake, 1: forward, -1: backward
bool g_forceBrake = false;  // true: force brake, false: normal operation

void motorBegin()
{
  pinMode(MOTOR_INX_LEFT_PIN, OUTPUT);
  pinMode(MOTOR_INY_LEFT_PIN, OUTPUT);
  pinMode(MOTOR_INX_RIGHT_PIN, OUTPUT);
  pinMode(MOTOR_INY_RIGHT_PIN, OUTPUT);
  pinMode(MOTOR_EN_LEFT_PIN, OUTPUT);
  pinMode(MOTOR_EN_RIGHT_PIN, OUTPUT);
  
  analogWrite(MOTOR_EN_LEFT_PIN, 0);
  analogWrite(MOTOR_EN_RIGHT_PIN, 0);
  digitalWrite(MOTOR_INX_LEFT_PIN, LOW);
  digitalWrite(MOTOR_INY_LEFT_PIN, LOW);
  digitalWrite(MOTOR_INX_RIGHT_PIN, LOW);
  digitalWrite(MOTOR_INY_RIGHT_PIN, LOW);

  // Info on LCD
#if USE_LCD == true
  displayMotorState();
#endif
}

void motorSpeed(int motorSpeed) // 0..255
{
  if (g_motorSpeed != motorSpeed)
  {
    g_motorSpeed = motorSpeed;
    analogWrite(MOTOR_EN_LEFT_PIN, motorSpeed);
    analogWrite(MOTOR_EN_RIGHT_PIN, motorSpeed);

    // Info on LCD
#if USE_LCD == true
    displayMotorState();
#endif

    // Debug print
    DPRINT(F("motorSpeed("));
    DPRINT(motorSpeed);
    DPRINTLN(F(")"));
  }
}

void motorLeftState(int motorState) // 0: brake, 1: forward, -1: backward
{
  if (g_motorLeftState != motorState)
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
      digitalWrite(MOTOR_INX_LEFT_PIN, LOW);
      digitalWrite(MOTOR_INY_LEFT_PIN, LOW);
    }

    // Info on LCD
#if USE_LCD == true
    displayMotorState();
#endif

    // Debug print
    DPRINT(F("motorLeftState("));
    DPRINT(motorState);
    DPRINTLN(F(")"));
  }
}

void motorRightState(int motorState) // 0: brake, 1: forward, -1: backward
{
  if (g_motorRightState != motorState)
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
      digitalWrite(MOTOR_INX_RIGHT_PIN, LOW);
      digitalWrite(MOTOR_INY_RIGHT_PIN, LOW);
    }

    // Info on LCD
#if USE_LCD == true
    displayMotorState();
#endif

    // Debug print
    DPRINT(F("motorRightState("));
    DPRINT(motorState);
    DPRINTLN(F(")"));
  }
}
