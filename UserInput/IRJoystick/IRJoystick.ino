/*
  IR Joystick

  - The weak IR remotes bundled with Arduino electronics, at best work 
    up to 3-5m. This project shows you how to make your own strong IR 
    remote. To control a RobotCar we usually continuously press a 
    direction button, but in case that the first code gets lost, the 
    repeat codes are not recognized and the car stops moving. For this 
    reason we avoid sending NEC repeat codes.

  - The project is powered by two 3.7V lithium cells in series. Please 
    handle the 18650 cells with care, the cheap battery holders have no 
    short-circuit and also no reverse polarity protection circuitry.
    
  - The Nano is connected to a push-button joystick module with 4 
    directions and to a 1W/3W IR module which switches the IR LED 
    through a n-channel MOSFET instead of the usual NPN BJT.

  - The Nano has only a small tantalum capacitor at its 5V LDO output 
    (see LM1117 or AMS1117). To achieve a stable boot of the Nano, I had 
    to add a 100μF electrolytic capacitor between GND and the 5V.
*/

// Disables static receiver code like receive timer ISR handler and static 
// IRReceiver and irparams data. Saves 450 bytes program memory and 269 bytes
// RAM if receiving functions are not required.
#if !defined(ARDUINO_ESP32C3_DEV) // this is due to a bug in RISC-V compiler, which requires unused function sections :-(
#define DISABLE_CODE_FOR_RECEIVER
#endif
#define NO_LED_FEEDBACK_CODE      // saves 566 bytes program memory
#define IR_SEND_PIN           12

#include <IRremote.hpp>           // IRremote by shirriff, z3t0, ArminJo, https://github.com/Arduino-IRremote/Arduino-IRremote

#define JOYSTICK_UP_PIN       8
#define JOYSTICK_DOWN_PIN     9
#define JOYSTICK_LEFT_PIN     10
#define JOYSTICK_RIGHT_PIN    11
#define DELAY_BETWEEN_CMD_MS  80  // a delay is needed, otherwise the rx sees it as one long signal

void setup()
{
  // Init pin modes
  pinMode(JOYSTICK_UP_PIN, INPUT_PULLUP);
  pinMode(JOYSTICK_LEFT_PIN, INPUT_PULLUP);
  pinMode(JOYSTICK_RIGHT_PIN, INPUT_PULLUP);
  pinMode(JOYSTICK_DOWN_PIN, INPUT_PULLUP);

  // Init IR sender with IR_SEND_PIN
  IrSender.begin();
}

void loop()
{
  // We do not use a switch statement because we want to support 
  // pressing two buttons at the same time, like:
  // UP+LEFT, UP+RIGHT, DOWN+LEFT or DOWN+RIGHT
  if (digitalRead(JOYSTICK_UP_PIN) == LOW)
  {
    IrSender.sendNEC(0, 0x46, 0);
    delay(DELAY_BETWEEN_CMD_MS);
  }
  if (digitalRead(JOYSTICK_LEFT_PIN) == LOW)
  {
    IrSender.sendNEC(0, 0x44, 0);
    delay(DELAY_BETWEEN_CMD_MS);
  }
  if (digitalRead(JOYSTICK_RIGHT_PIN) == LOW)
  {
    IrSender.sendNEC(0, 0x43, 0);
    delay(DELAY_BETWEEN_CMD_MS);
  }
  if (digitalRead(JOYSTICK_DOWN_PIN) == LOW)
  {
    IrSender.sendNEC(0, 0x15, 0);
    delay(DELAY_BETWEEN_CMD_MS);
  }
}
