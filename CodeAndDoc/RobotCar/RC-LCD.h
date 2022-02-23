/*  
  16x2 LCD Display over I2C
*/
#ifndef rc_lcd_h
#define rc_lcd_h

#include "RC-Config.h"

#if USE_LCD == 1

#include <LiquidCrystal_I2C.h>

const char arrow_left_char[] = {0b00000, 0b00100, 0b01100, 0b11111, 0b01100, 0b00100, 0b00000, 0b00000};
const char arrow_right_char[] = {0b00000, 0b00100, 0b00110, 0b11111, 0b00110, 0b00100, 0b00000, 0b00000};
const char arrow_up_char[] = {0b00100, 0b01110, 0b11111, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100};
const char arrow_down_char[] = {0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b11111, 0b01110, 0b00100};

extern LiquidCrystal_I2C g_lcd;

extern void displayMode();
extern void displayMotorDirection();

#endif
#endif
