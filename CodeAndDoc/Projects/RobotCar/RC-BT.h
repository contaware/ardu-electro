/*  
  Bluetooth ZS-040 (HC-05 or HC-06) module
  
  Using App:
  Bluetooth RC Joystick Controller (Control Joystick) by Andi.Co
  
  Pairing code:
  1234
  
  Level shifter:
  Remember to use a level shifter for RX and TX or at least a voltage 
  divider from the Arduino TX pin (5V) to the module RX pin (3.3V)
  (1K and 2K resistors give a good divider)

  Attention:
  Do not use the SoftwareSerial library as it conflicts with the Servo library,
  it's always best to only use the hardware serial.
  
  Site:
  https://sites.google.com/site/bluetoothrccar/1-parts-needed-and-the-arduino-motor-shield
  
  ELEGOO BT16 module is not working:
  https://community.appinventor.mit.edu/t/cant-connect-to-elegoo-bt16-module/
*/
#ifndef rc_bt_h
#define rc_bt_h

#include "RC-Config.h"

#if USE_BLUETOOTH == true

const byte BLUETOOTH_FORWARD_MIN = 1;                   // can be changed in the App
const byte BLUETOOTH_FORWARD_MAX = 255;                 // can be changed in the App
const byte BLUETOOTH_BACKWARD_MIN = 1;                  // can be changed in the App
const byte BLUETOOTH_BACKWARD_MAX = 255;                // can be changed in the App
const byte BLUETOOTH_CENTER = 88;                       // can be changed in the App
const byte BLUETOOTH_LEFT_MIN = BLUETOOTH_CENTER - 1;   // can be changed in the App
const byte BLUETOOTH_LEFT_MAX = 65;                     // can be changed in the App
const byte BLUETOOTH_RIGHT_MIN = BLUETOOTH_CENTER + 1;  // can be changed in the App
const byte BLUETOOTH_RIGHT_MAX = 110;                   // can be changed in the App
const byte BLUETOOTH_TURN_SPEED_MIN = 130;              // 0..255
const byte BLUETOOTH_TURN_SPEED_MAX = 230;              // 0..255
const byte BLUETOOTH_SPEED_MIN = 100;                   // 0..255
const byte BLUETOOTH_SPEED_MAX = 255;                   // 0..255
const unsigned long BLUETOOTH_CONNECTION_TIMEOUT = 200; // ms

extern void bluetoothBegin();
extern void bluetoothControl();

#endif
#endif
