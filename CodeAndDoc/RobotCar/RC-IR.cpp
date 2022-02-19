#include "RC-IR.h"

#if USE_IR_RECEIVER == 1

#include <IRremote.hpp>
#include "RC-Motor.h"

unsigned long irPrevMs;
unsigned long lastKeyValue = 0;

void irBegin()
{
  IrReceiver.begin(IR_RECEIVE_PIN, DISABLE_LED_FEEDBACK); // pass DISABLE_LED_FEEDBACK to leave LED pin usable
  irPrevMs = millis();
}

void irControl()
{
  if (IrReceiver.decode()) // have we received an IR signal?
  {  
    // Print info
    //IrReceiver.printIRResultShort(&Serial);

    if (IrReceiver.decodedIRData.protocol == NEC)
    {
      // Get key value
      unsigned long currentKeyValue = IrReceiver.decodedIRData.command;
      if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT)
          currentKeyValue = lastKeyValue;

      // Valid?
      if (currentKeyValue != 0)
      {
        switch (currentKeyValue)
        {
          case IR_REMOTE_FORWARD:
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
            irPrevMs = millis();
            break;
          }
          case IR_REMOTE_KEY1:          DEBUG_SERIAL.println(F("Key1")); break;
          case IR_REMOTE_KEY2:          DEBUG_SERIAL.println(F("Key2")); break;
          case IR_REMOTE_KEY3:          DEBUG_SERIAL.println(F("Key3")); break;
          case IR_REMOTE_KEY4:          DEBUG_SERIAL.println(F("Key4")); break;
          case IR_REMOTE_KEY5:          DEBUG_SERIAL.println(F("Key5")); break;
          case IR_REMOTE_KEY6:          DEBUG_SERIAL.println(F("Key6")); break;
          case IR_REMOTE_KEY7:          DEBUG_SERIAL.println(F("Key7")); break;
          case IR_REMOTE_KEY8:          DEBUG_SERIAL.println(F("Key8")); break;
          case IR_REMOTE_KEY9:          DEBUG_SERIAL.println(F("Key9")); break;
          case IR_REMOTE_KEY0:          DEBUG_SERIAL.println(F("Key0")); break;
          case IR_REMOTE_KEY_STAR:      DEBUG_SERIAL.println(F("Star")); break;
          case IR_REMOTE_KEY_HASH:      DEBUG_SERIAL.println(F("Hash")); break;
          default:                      DEBUG_SERIAL.println(currentKeyValue); break;
        }
        lastKeyValue = currentKeyValue;
      }
      else
        lastKeyValue = 0;
    }
    else
      lastKeyValue = 0;
      
    // Enable receiving of the next value
    IrReceiver.resume();
  }

  // Stop?
  if (millis() - irPrevMs > IR_KEYSEND_TIMEOUT)
  {
    motorSpeed(0);
    irPrevMs = millis();
  }
}

#endif
