#include "RC-IR.h"

#define DECODE_NEC      // specify which protocol(s) should be used for decoding (if no protocol is defined, all protocols are active)
#include <IRremote.hpp> // IRremote by shirriff, z3t0, ArminJo, https://github.com/Arduino-IRremote/Arduino-IRremote
#include "RC-Motor.h"
#include "RC-Servo.h"

unsigned long irPrevMs;
long lastKeyValue = -1; // -1 means invalid or not set

void irBegin()
{
  IrReceiver.begin(IR_RECEIVE_PIN, DISABLE_LED_FEEDBACK); // pass DISABLE_LED_FEEDBACK to leave LED pin usable
  irPrevMs = millis();
}

void irControl()
{
  // Check if received data is available and if yes, try to decode it.
  // The decoded result is placed in the IrReceiver.decodedIRData structure.
  if (IrReceiver.decode())
  {
    // Enable receiving of the next IR frame
    IrReceiver.resume();

    if (IrReceiver.decodedIRData.protocol == NEC)
    {
      // Get key value
      long currentKeyValue;
      if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT)
        currentKeyValue = lastKeyValue;
      else
        currentKeyValue = IrReceiver.decodedIRData.command; // command is a uint16_t

      // Valid?
      if (currentKeyValue >= 0)
      {
        switch (currentKeyValue)
        {
          case IR_REMOTE_FORWARD:
          {
            if (g_forceBrake)
            {
              // Brake
              motorLeftState(0);
              motorRightState(0);
              motorSpeed(255);
            }
            else
            {
              // Forward
              if (g_motorLeftState > 0 && g_motorRightState > 0)
                motorSpeed(constrain(g_motorSpeed + IR_REMOTE_SPEED_INC, IR_REMOTE_SPEED_MIN, IR_REMOTE_SPEED_MAX));
              else
              {
                motorSpeed(IR_REMOTE_SPEED_MIN);
                motorLeftState(1);
                motorRightState(1);
              }
            }
            irPrevMs = millis();
            break;
          }
          case IR_REMOTE_BACKWARD:
          {
            // Backward
            if (g_motorLeftState < 0 && g_motorRightState < 0)
              motorSpeed(constrain(g_motorSpeed + IR_REMOTE_SPEED_INC, IR_REMOTE_SPEED_MIN, IR_REMOTE_SPEED_MAX));
            else
            {
              motorSpeed(IR_REMOTE_SPEED_MIN);
              motorLeftState(-1);
              motorRightState(-1);
            }
            irPrevMs = millis();
            break;
          }
          case IR_REMOTE_LEFT:
          {
            // Left turn
            if (g_motorLeftState < 0 && g_motorRightState > 0)
              motorSpeed(constrain(g_motorSpeed + IR_REMOTE_SPEED_INC, IR_REMOTE_TURN_SPEED_MIN, IR_REMOTE_TURN_SPEED_MAX));
            else
            {
              motorSpeed(IR_REMOTE_TURN_SPEED_MIN);
              motorLeftState(-1);
              motorRightState(1);
            }
            irPrevMs = millis();
            break;
          }
          case IR_REMOTE_RIGHT:
          {
            // Right turn
            if (g_motorLeftState > 0 && g_motorRightState < 0)
              motorSpeed(constrain(g_motorSpeed + IR_REMOTE_SPEED_INC, IR_REMOTE_TURN_SPEED_MIN, IR_REMOTE_TURN_SPEED_MAX));
            else
            {
              motorSpeed(IR_REMOTE_TURN_SPEED_MIN);
              motorLeftState(1);
              motorRightState(-1);
            }
            irPrevMs = millis();
            break;
          }
          case IR_REMOTE_OK:
          {
            // Brake
            motorLeftState(0);
            motorRightState(0);
            motorSpeed(255);
            irPrevMs = millis();
            break;
          }
          case IR_REMOTE_KEY1:
          case IR_REMOTE_KEY4:
          case IR_REMOTE_KEY7:
          case IR_REMOTE_KEY_STAR:

            if ((IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) == 0)
            {
#if USE_SERVO == true
              g_servo.write(100);
#endif
            }
            break;
          case IR_REMOTE_KEY2:
          case IR_REMOTE_KEY5:
          case IR_REMOTE_KEY8:
          case IR_REMOTE_KEY0:
            if ((IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) == 0)
            {
#if USE_SERVO == true
              g_servo.write(90);
#endif
            }
            break;
          case IR_REMOTE_KEY3:
          case IR_REMOTE_KEY6:
          case IR_REMOTE_KEY9:
          case IR_REMOTE_KEY_HASH:
            if ((IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) == 0)
            {
#if USE_SERVO == true
              g_servo.write(80);
#endif
            }
            break;
          default:
            break;
        }
        lastKeyValue = currentKeyValue;
      }
      else
        lastKeyValue = -1;
    }
    else
      lastKeyValue = -1;
  }

  // Stop?
  if (millis() - irPrevMs > IR_KEYSEND_TIMEOUT)
  {
    motorSpeed(0);
    irPrevMs = millis();
  }
}
