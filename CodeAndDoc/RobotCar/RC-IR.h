/*  
  IR Receiver KY-022 (1838) module
*/
#ifndef rc_ir_h
#define rc_ir_h

#include "RC-Config.h"

#if USE_IR_RECEIVER == 1

#define IR_REMOTE_FORWARD         0x46
#define IR_REMOTE_BACKWARD        0x15
#define IR_REMOTE_LEFT            0x44
#define IR_REMOTE_RIGHT           0x43
#define IR_REMOTE_OK              0x40
#define IR_REMOTE_KEY1            0x16
#define IR_REMOTE_KEY2            0x19
#define IR_REMOTE_KEY3            0xD
#define IR_REMOTE_KEY4            0xC
#define IR_REMOTE_KEY5            0x18
#define IR_REMOTE_KEY6            0x5E
#define IR_REMOTE_KEY7            0x8
#define IR_REMOTE_KEY8            0x1C
#define IR_REMOTE_KEY9            0x5A
#define IR_REMOTE_KEY0            0x52
#define IR_REMOTE_KEY_STAR        0x42
#define IR_REMOTE_KEY_HASH        0x4A

const int IR_REMOTE_TURN_SPEED_MIN = 130;     // 0..255
const int IR_REMOTE_TURN_SPEED_MAX = 230;     // 0..255
const int IR_REMOTE_SPEED_MIN = 100;          // 0..255
const int IR_REMOTE_SPEED_MAX = 255;          // 0..255
const int IR_REMOTE_SPEED_INC = 10;
const unsigned long IR_KEYSEND_TIMEOUT = 200; // ms

extern void irBegin();
extern void irControl();

#endif
#endif
