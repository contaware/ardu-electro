#include "RC-LCD.h"

#if USE_LCD == true

#include "RC-Motor.h"

LiquidCrystal_I2C g_lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 lines display

void displayMode()
{
  g_lcd.setCursor(0, 0);
  if (g_mode == 0)
    g_lcd.print("0.Motors OFF    ");
  else if (g_mode == 1)
    g_lcd.print("1.Motors IR     ");
  else if (g_mode == 2)
    g_lcd.print("2.Motors BT     ");
  else if (g_mode == 3)
    g_lcd.print("3.Motors PS2    ");
  else if (g_mode == 4)
    g_lcd.print("4.Line tracking ");
  else if (g_mode == 5)
    g_lcd.print("5.Obstacle avoid");
  else
    g_lcd.print("Unknown mode    ");
  g_lcd.setCursor(0, 1);
  g_lcd.print("                ");
}

void displayMotorDirection()
{
  if (g_motorSpeed == 0)
    g_lcd.write(' '); // stop
  else if (g_motorLeftState == 1 && g_motorRightState == 1)
    g_lcd.write(2);   // up arrow
  else if (g_motorLeftState == -1 && g_motorRightState == -1)
    g_lcd.write(3);   // down arrow
  else if (g_motorLeftState == 1 && g_motorRightState == -1)
    g_lcd.write(1);   // right arrow
  else if (g_motorLeftState == -1 && g_motorRightState == 1)
    g_lcd.write(0);   // left arrow
  else
    g_lcd.write(' '); // brake
}

#endif
