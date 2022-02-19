#include "RC-LCD.h"

#if USE_LCD == 1

LiquidCrystal_I2C g_lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 lines display

#endif
