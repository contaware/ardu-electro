/*
  The HD44780 controller has 80 chars of display RAM, so for a 16x2 display the first line can contain
  additional 40 - 16 = 24 hidden chars per line. The chars wrap from the first to the second line and
  then back to the first.
  http://web.alfredstate.edu/faculty/weimandn/lcd/lcd_addressing/lcd_addressing_index.html
  To see the hidden chars use:
  scrollDisplayLeft() / scrollDisplayRight()
  autoscroll() / noAutoscroll()
*/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 lines display

// Custom chars 5 x 8 dots
// https://arduinogetstarted.com/faq/how-to-use-special-character-on-lcd
const char bell_char[] = {0b00100, 0b01110, 0b01110, 0b01110, 0b11111, 0b00000, 0b00100, 0b00000};
const char heart_char[] = {0b00000, 0b01010, 0b11111, 0b11111, 0b01110, 0b00100, 0b00000, 0b00000};
const char smiley_char[] = {0b00000, 0b10001, 0b00000, 0b00000, 0b10001, 0b01110, 0b00000};
const char check_char[] = {0b00000, 0b00001, 0b00011, 0b10110, 0b11100, 0b01000, 0b00000, 0b00000};
const char arrow_left_char[] = {0b00000, 0b00100, 0b01100, 0b11111, 0b01100, 0b00100, 0b00000, 0b00000};
const char arrow_right_char[] = {0b00000, 0b00100, 0b00110, 0b11111, 0b00110, 0b00100, 0b00000, 0b00000};
const char arrow_up_char[] = {0b00100, 0b01110, 0b11111, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100};
const char arrow_down_char[] = {0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b11111, 0b01110, 0b00100};

void setup()
{
  lcd.init();
  lcd.backlight(); // turn on the backlight

  // We can create a max of 8 custom chars
  lcd.createChar(0, bell_char);
  lcd.createChar(1, heart_char);
  lcd.createChar(2, smiley_char);
  lcd.createChar(3, check_char);
  lcd.createChar(4, arrow_left_char);
  lcd.createChar(5, arrow_right_char);
  lcd.createChar(6, arrow_up_char);
  lcd.createChar(7, arrow_down_char);
  
  // Display all 256 chars
  uint8_t c = 0;
  for (int i = 0 ; i < 8 ; i++)
  {
    int x;
    lcd.clear();
    lcd.setCursor(0, 0);
    for (x = 0 ; x < 16 ; x++)  
      lcd.write(c++);
    lcd.setCursor(0, 1);
    for (x = 0 ; x < 16 ; x++)  
      lcd.write(c++);
    delay(3000);
  }

  // Init
  lcd.clear(); // returns to home position and clears everything, while home() just returns to home position
  lcd.setCursor(0, 0); // column, row
  lcd.print("Hello, world!");
  lcd.setCursor(0, 1);
  lcd.write(0);
  lcd.write(1);
  lcd.write(2);
  lcd.write(3);
  lcd.write(4);
  lcd.write(5);
  lcd.write(6);
  lcd.write(7);
}

void loop()
{
  delay(1000);
  lcd.home();
  lcd.noBlink();
  lcd.cursor();
  for (int i = 0 ; i < 16 ; i++)
  {
    lcd.setCursor(i, 0);
    delay(300);
  }
  lcd.blink();
  lcd.noCursor(); // note: blink + cursor could also be set together
  delay(1000);
  for (int i = 15 ; i >= 0 ; i--)
  {
    lcd.setCursor(i, 0);
    delay(300);
  }
  delay(1000);
  lcd.noDisplay(); // to hide all chars
  for (int i = 0 ; i < 16 ; i++)
    lcd.scrollDisplayLeft(); // scroll entire display, chars are not lost!
  lcd.display();
  for (int i = 0 ; i < 16 ; i++)
  {
    lcd.scrollDisplayRight(); // scroll entire display, chars are not lost!
    delay(230);
  }
}
