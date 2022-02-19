#include "RC-BT.h"

#if USE_BLUETOOTH == 1

#include "RC-Motor.h"

unsigned long bluetoothPrevMs;
byte commands[4];

void bluetoothBegin()
{
  bluetoothPrevMs = millis();
}

void bluetoothControl()
{
  if (Serial.available() >= 4)
  {
    commands[0] = Serial.read();  // direction
    commands[1] = Serial.read();  // speed
    commands[2] = Serial.read();  // angle
    commands[3] = Serial.read();  // lights and buttons states
    if (commands[0] >= 0xf1 && commands[0] <= 0xf3)
    {
      // Left turn
      if (commands[2] < BLUETOOTH_CENTER)
      {
        motorSpeed(map(commands[2], BLUETOOTH_LEFT_MIN, BLUETOOTH_LEFT_MAX, BLUETOOTH_TURN_SPEED_MIN, BLUETOOTH_TURN_SPEED_MAX));
        motorLeftState(-1);
        motorRightState(1);
      }
      // Right turn
      else if (commands[2] > BLUETOOTH_CENTER)
      {
        motorSpeed(map(commands[2], BLUETOOTH_RIGHT_MIN, BLUETOOTH_RIGHT_MAX, BLUETOOTH_TURN_SPEED_MIN, BLUETOOTH_TURN_SPEED_MAX));
        motorLeftState(1);
        motorRightState(-1);
      }
      // Forward
      else if (commands[0] == 0xf1)
      {
        motorSpeed(map(commands[1], BLUETOOTH_FORWARD_MIN, BLUETOOTH_FORWARD_MAX, BLUETOOTH_SPEED_MIN, BLUETOOTH_SPEED_MAX));
        motorLeftState(1);
        motorRightState(1);
      }
      // Backward
      else if (commands[0] == 0xf2)
      {
        motorSpeed(map(commands[1], BLUETOOTH_BACKWARD_MIN, BLUETOOTH_BACKWARD_MAX, BLUETOOTH_SPEED_MIN, BLUETOOTH_SPEED_MAX));
        motorLeftState(-1);
        motorRightState(-1);
      }
      // Stop
      else // 0xf3
        motorSpeed(0);
    }
    // Tilt pan (we do not use that, just stop)
    else if (commands[0] == 0xf4)
      motorSpeed(0);
    // Stop all
    else if (commands[0] == 0xf5)
      motorSpeed(0);
    // In case the bytes are not being read in correct order
    else
    {
      motorSpeed(0);
      Serial.end();               // reset the Serial port clearing the buffer 
      Serial.begin(SERIAL_SPEED); // re-open it
    }

    // Heart beat
    bluetoothPrevMs = millis();
  }

  // Stop motors and re-connect in case that the connection is interrupted
  if (millis() - bluetoothPrevMs > BLUETOOTH_CONNECTION_TIMEOUT)
  {
    motorSpeed(0);
    Serial.end();               // reset the Serial port clearing the buffer 
    Serial.begin(SERIAL_SPEED); // re-open it
    bluetoothPrevMs = millis();
  }
}

#endif
