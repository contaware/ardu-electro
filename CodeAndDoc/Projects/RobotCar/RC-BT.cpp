#include "RC-BT.h"

#if USE_BLUETOOTH == true

#include "RC-Motor.h"

unsigned long bluetoothPrevMs;
byte commands[4];
byte prevCommands[4] = {0, 0, 0, 0};

void bluetoothBegin()
{
  bluetoothPrevMs = millis();
}

void bluetoothControl()
{
  if (BLUETOOTH_SERIAL.available() >= 4)
  {
    commands[0] = BLUETOOTH_SERIAL.read();  // direction
    commands[1] = BLUETOOTH_SERIAL.read();  // speed
    commands[2] = BLUETOOTH_SERIAL.read();  // angle
    commands[3] = BLUETOOTH_SERIAL.read();  // lights and buttons states
    if (commands[0] >= 0xf1 && commands[0] <= 0xf3)
    {
      // Cycle mode (horn button pressed)
      if (bitRead(commands[3], 5) && !bitRead(prevCommands[3], 5)) // if changed from 0 -> 1
        setMode(g_mode + 1);
        
      // Left turn
      if (commands[2] < BLUETOOTH_CENTER)
      {
        if (g_mode == 2)
        {
          motorSpeed(map(commands[2], BLUETOOTH_LEFT_MIN, BLUETOOTH_LEFT_MAX, BLUETOOTH_TURN_SPEED_MIN, BLUETOOTH_TURN_SPEED_MAX));
          motorLeftState(-1);
          motorRightState(1);
        }
      }
      // Right turn
      else if (commands[2] > BLUETOOTH_CENTER)
      {
        if (g_mode == 2)
        {
          motorSpeed(map(commands[2], BLUETOOTH_RIGHT_MIN, BLUETOOTH_RIGHT_MAX, BLUETOOTH_TURN_SPEED_MIN, BLUETOOTH_TURN_SPEED_MAX));
          motorLeftState(1);
          motorRightState(-1);
        }
      }
      // Forward
      else if (commands[0] == 0xf1)
      {
        if (g_mode == 2)
        {
          motorSpeed(map(commands[1], BLUETOOTH_FORWARD_MIN, BLUETOOTH_FORWARD_MAX, BLUETOOTH_SPEED_MIN, BLUETOOTH_SPEED_MAX));
          motorLeftState(1);
          motorRightState(1);
        }
      }
      // Backward
      else if (commands[0] == 0xf2)
      {
        if (g_mode == 2)
        {
          motorSpeed(map(commands[1], BLUETOOTH_BACKWARD_MIN, BLUETOOTH_BACKWARD_MAX, BLUETOOTH_SPEED_MIN, BLUETOOTH_SPEED_MAX));
          motorLeftState(-1);
          motorRightState(-1);
        }
      }
      // Stop
      else // 0xf3
      {
        if (g_mode == 2)
          motorSpeed(0);
      }
    }
    // Tilt pan (we do not use that, just stop)
    else if (commands[0] == 0xf4)
    {
      // Cycle mode (horn button pressed)
      if (bitRead(commands[3], 5) && !bitRead(prevCommands[3], 5)) // if changed from 0 -> 1
        setMode(g_mode + 1);
        
      if (g_mode == 2)
        motorSpeed(0);
    }
    // Stop all
    else if (commands[0] == 0xf5)
    {
      if (g_mode == 2)
        motorSpeed(0);
    }
    // In case the bytes are not being read in correct order
    else
    {
      if (g_mode == 2)
        motorSpeed(0);
      BLUETOOTH_SERIAL.end();                         // reset the Serial port clearing the buffer 
      BLUETOOTH_SERIAL.begin(BLUETOOTH_SERIAL_SPEED); // re-open it
    }
    
    // Store commands  
    memcpy(prevCommands, commands, 4);
    
    // Heart beat
    bluetoothPrevMs = millis();
  }

  // Stop motors and re-connect in case that the connection is interrupted
  if (millis() - bluetoothPrevMs > BLUETOOTH_CONNECTION_TIMEOUT)
  {
    if (g_mode == 2)
      motorSpeed(0);
    BLUETOOTH_SERIAL.end();                         // reset the Serial port clearing the buffer 
    BLUETOOTH_SERIAL.begin(BLUETOOTH_SERIAL_SPEED); // re-open it
    bluetoothPrevMs = millis();
  }
}

#endif
