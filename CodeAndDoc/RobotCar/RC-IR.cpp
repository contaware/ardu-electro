#include "RC-IR.h"

#if USE_IR_RECEIVER == true

#define DECODE_NEC      // specify which protocol(s) should be used for decoding (if no protocol is defined, all protocols are active)
#include <IRremote.hpp>
#include "RC-Motor.h"

unsigned long irPrevMs;
long lastKeyValue = -1; // -1 means invalid or not set

void irBegin()
{
  IrReceiver.begin(IR_RECEIVE_PIN, DISABLE_LED_FEEDBACK); // pass DISABLE_LED_FEEDBACK to leave LED pin usable
  irPrevMs = millis();
}

void irControl()
{
  // Have we received an IR signal?
  if (IrReceiver.decode())
  {
    if (IrReceiver.decodedIRData.protocol == NEC)
    {
      // Get key value
      long currentKeyValue = IrReceiver.decodedIRData.command; // command is a uint16_t
      if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT)
          currentKeyValue = lastKeyValue;

      // Valid?
      if (currentKeyValue >= 0)
      {
        switch (currentKeyValue)
        {
          case IR_REMOTE_FORWARD:
          {
            // Forward
            if (g_mode == 1)
            {
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
            if (g_mode == 1)
            {
              if (g_motorLeftState < 0 && g_motorRightState < 0)
                motorSpeed(constrain(g_motorSpeed + IR_REMOTE_SPEED_INC, IR_REMOTE_SPEED_MIN, IR_REMOTE_SPEED_MAX));
              else
              {
                motorSpeed(IR_REMOTE_SPEED_MIN);
                motorLeftState(-1);
                motorRightState(-1);
              }
            }
            irPrevMs = millis();
            break;
          }
          case IR_REMOTE_LEFT:
          {
            // Left turn
            if (g_mode == 1)
            {
              if (g_motorLeftState < 0 && g_motorRightState > 0)
                motorSpeed(constrain(g_motorSpeed + IR_REMOTE_SPEED_INC, IR_REMOTE_TURN_SPEED_MIN, IR_REMOTE_TURN_SPEED_MAX));
              else
              {
                motorSpeed(IR_REMOTE_TURN_SPEED_MIN);
                motorLeftState(-1);
                motorRightState(1);
              }
            }
            irPrevMs = millis();
            break;
          }
          case IR_REMOTE_RIGHT:
          {
            // Right turn
            if (g_mode == 1)
            {
              if (g_motorLeftState > 0 && g_motorRightState < 0)
                motorSpeed(constrain(g_motorSpeed + IR_REMOTE_SPEED_INC, IR_REMOTE_TURN_SPEED_MIN, IR_REMOTE_TURN_SPEED_MAX));
              else
              {
                motorSpeed(IR_REMOTE_TURN_SPEED_MIN);
                motorLeftState(1);
                motorRightState(-1);
              }
            }
            irPrevMs = millis();
            break;
          }
          case IR_REMOTE_OK:
          {
            // Brake
            if (g_mode == 1)
            {
              motorLeftState(0);
              motorRightState(0);
            }
            irPrevMs = millis();
            break;
          }
          case IR_REMOTE_KEY1:
            if ((IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) == 0)
              setMode(1);
            break;
          case IR_REMOTE_KEY2:
            if ((IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) == 0)
              setMode(2);
            break;
          case IR_REMOTE_KEY3:
            if ((IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) == 0)
              setMode(3);
            break;
          case IR_REMOTE_KEY4:
            if ((IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) == 0)
              setMode(4);
            break;
          case IR_REMOTE_KEY5:
            if ((IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) == 0)
              setMode(5);
            break;
          case IR_REMOTE_KEY0:
            if ((IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) == 0)
              setMode(0);
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
      
    // Enable receiving of the next value
    IrReceiver.resume();
  }

  // Stop?
  if (millis() - irPrevMs > IR_KEYSEND_TIMEOUT)
  {
    if (g_mode == 1)
      motorSpeed(0);
    irPrevMs = millis();
  }
}

#endif
