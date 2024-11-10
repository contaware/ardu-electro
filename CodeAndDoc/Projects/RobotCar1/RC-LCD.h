/*  
  16x2 LCD Display over I2C
*/
#ifndef rc_lcd_h
#define rc_lcd_h

#include "RC-Config.h"

#if USE_LCD == true

#include <LiquidCrystal_I2C.h> // LiquidCrystal I2C by Frank de Brabander, https://github.com/johnrickman/LiquidCrystal_I2C

extern LiquidCrystal_I2C g_lcd;

extern void lcdBegin();
extern void displayMotorState();
extern void displayRpmSpeed(unsigned long rpmAvg, double speedAvg);

#endif
#endif
