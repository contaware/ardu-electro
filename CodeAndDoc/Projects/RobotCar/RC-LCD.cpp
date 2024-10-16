#include "RC-LCD.h"

#if USE_LCD == true

#include "RC-Motor.h"

// The library wants uint8_t and not const...
uint8_t arrow_left_char[] = {0b00000, 0b00100, 0b01100, 0b11111, 0b01100, 0b00100, 0b00000, 0b00000};
uint8_t arrow_right_char[] = {0b00000, 0b00100, 0b00110, 0b11111, 0b00110, 0b00100, 0b00000, 0b00000};
uint8_t arrow_up_char[] = {0b00100, 0b01110, 0b11111, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100};
uint8_t arrow_down_char[] = {0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b11111, 0b01110, 0b00100};

LiquidCrystal_I2C g_lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 lines display

void lcdBegin()
{
  g_lcd.init();
  g_lcd.backlight();          // turn on the backlight
  g_lcd.createChar(0, arrow_left_char);
  g_lcd.createChar(1, arrow_right_char);
  g_lcd.createChar(2, arrow_up_char);
  g_lcd.createChar(3, arrow_down_char);
  g_lcd.clear();              // returns to home position and clears everything, while home() just returns to home position
}

void displayMotorState()
{
  g_lcd.setCursor(0, 0);

  // Direction
  if (g_motorSpeed == 0)
    g_lcd.print("         "); // stop
  else if (g_motorLeftState == 1 && g_motorRightState == 1)
  {
    g_lcd.write(2);           // up arrow
    g_lcd.print("        ");
  }
  else if (g_motorLeftState == -1 && g_motorRightState == -1)
  {
    g_lcd.write(3);           // down arrow
    g_lcd.print("        ");
  }
  else if (g_motorLeftState == 1 && g_motorRightState == -1)
  {
    g_lcd.write(1);           // right arrow
    g_lcd.print("        ");
  }
  else if (g_motorLeftState == -1 && g_motorRightState == 1)
  {
    g_lcd.write(0);           // left arrow
    g_lcd.print("        ");
  }
  else
    g_lcd.print("Braking  ");

  // PWM
  char s[8];
  sprintf(s, "PWM=%03d", g_motorSpeed);
  g_lcd.print(s);
}

void displayRpmSpeed(unsigned long rpmAvg, double speedAvg)
{
  g_lcd.setCursor(0, 1);

  char s[17];
  char speedAvgStr[5];
  sprintf(s, "%3lurpm   %sm/s", rpmAvg, dtostrf(speedAvg, 4, 2, speedAvgStr));
  g_lcd.print(s);
}

#endif
