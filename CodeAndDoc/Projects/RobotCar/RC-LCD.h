/*  
  16x2 LCD Display over I2C
*/
#ifndef rc_lcd_h
#define rc_lcd_h

#include "RC-Config.h"

#if USE_LCD == true

#include <LiquidCrystal_I2C.h> // LiquidCrystal I2C by Frank de Brabander, https://github.com/johnrickman/LiquidCrystal_I2C

// The library wants uint8_t and not const...
extern uint8_t arrow_left_char[];
extern uint8_t arrow_right_char[];
extern uint8_t arrow_up_char[];
extern uint8_t arrow_down_char[];

extern LiquidCrystal_I2C g_lcd;

extern void displayMotorState();
extern void displayRpmSpeed(unsigned long rpmAvg, double speedAvg);

#endif
#endif
