/*  
  16x2 LCD Display over I2C
*/
#ifndef rc_lcd_h
#define rc_lcd_h

#include "RC-Config.h"

#if USE_LCD == 1

#include <LiquidCrystal_I2C.h>

extern LiquidCrystal_I2C g_lcd;

extern void displayMode();

#endif
#endif
