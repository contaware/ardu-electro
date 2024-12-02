/*
  LCD modules based on the HD44780 chip with I2C support through PCF8574
  
  - VDD supply is 5V.
  
  - The HD44780 controller has 80 chars of display RAM, so for a 16x2 
    display the first line can contain additional 40 - 16 = 24 hidden 
    chars per line. The chars wrap from the first to the second line and 
    then back to the first.
    To see the hidden chars use: scrollDisplayLeft() / scrollDisplayRight()
                                 autoscroll() / noAutoscroll()
    https://web.alfredstate.edu/faculty/weimandn/lcd/lcd_addressing/lcd_addressing_index.html
*/
#include <LiquidCrystal_I2C.h> // LiquidCrystal I2C by Frank de Brabander
                               // https://github.com/johnrickman/LiquidCrystal_I2C

const uint8_t LCD_COLS = 16;  // 16 or 20
const uint8_t LCD_ROWS = 2;   // 2  or 4
LiquidCrystal_I2C lcd(0x27, LCD_COLS, LCD_ROWS); // PCF8574: 0x20 - 0x27 (default)
                                                 // PCF8574A: 0x38 - 0x3F (default)

// Custom chars 5 x 8 dots
// https://arduinogetstarted.com/faq/how-to-use-special-character-on-lcd
uint8_t bell_char[] = {0b00100, 0b01110, 0b01110, 0b01110, 0b11111, 0b00000, 0b00100, 0b00000};
uint8_t heart_char[] = {0b00000, 0b01010, 0b11111, 0b11111, 0b01110, 0b00100, 0b00000, 0b00000};
uint8_t smiley_char[] = {0b00000, 0b10001, 0b00000, 0b00000, 0b10001, 0b01110, 0b00000};
uint8_t check_char[] = {0b00000, 0b00001, 0b00011, 0b10110, 0b11100, 0b01000, 0b00000, 0b00000};
uint8_t arrow_left_char[] = {0b00000, 0b00100, 0b01100, 0b11111, 0b01100, 0b00100, 0b00000, 0b00000};
uint8_t arrow_right_char[] = {0b00000, 0b00100, 0b00110, 0b11111, 0b00110, 0b00100, 0b00000, 0b00000};
uint8_t arrow_up_char[] = {0b00100, 0b01110, 0b11111, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100};
uint8_t arrow_down_char[] = {0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b11111, 0b01110, 0b00100};

void displayAllChars(unsigned long delayMs)
{
  lcd.clear();
  uint8_t c = 0;
  int row = 0;
  while (true)
  {
    // Set position
    lcd.setCursor(0, row);

    // Display row
    for (int x = 0 ; x < LCD_COLS ; x++)
    {
      // Write char
      lcd.write(c);

      // Last char written?
      if (c == 255)
      {
        delay(delayMs);
        lcd.clear();
        return;
      }
      else
        c++;
    }

    // Next row
    row = (row + 1) % LCD_ROWS;

    // If display has been filled 
    // wait a moment and then clear it 
    if (row == 0)
    {
      delay(delayMs);
      lcd.clear();
    }
  }
}

void setup()
{
  // Init
  lcd.init();             // this calls Wire.begin() which sets the default clock of 100000 (100kHz)
  Wire.setClock(100000);  // 100000 (standard mode), 400000 (fast mode)
  lcd.backlight();        // turn on the backlight

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
  displayAllChars(5000);

  // Speed test for setCursor()
  unsigned long startMicros, endMicros;
  lcd.clear(); // returns to home position and clears everything, while home() just returns to home position
  startMicros = micros();
  lcd.setCursor(0, 0);
  endMicros = micros();
  lcd.print("cursor ");
  lcd.print(endMicros - startMicros);
  lcd.print("us");
  delay(5000);

  // Speed test for write()
  lcd.clear(); // returns to home position and clears everything, while home() just returns to home position
  startMicros = micros();
  lcd.write('A');
  endMicros = micros();
  lcd.setCursor(0, 0);
  lcd.print("write ");
  lcd.print(endMicros - startMicros);
  lcd.print("us");
  delay(5000);

  // Speed test for a 4 characters print()
  lcd.clear(); // returns to home position and clears everything, while home() just returns to home position
  startMicros = micros();
  lcd.print("FOUR");
  endMicros = micros();
  lcd.setCursor(0, 0);
  lcd.print("print ");
  lcd.print(endMicros - startMicros);
  lcd.print("us");
  delay(5000);
  
  // Print to all rows
  lcd.clear(); // returns to home position and clears everything, while home() just returns to home position
  lcd.setCursor(0, 0); // column, row
  lcd.print("Hello...");
  if (LCD_ROWS > 1)
  {
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
  if (LCD_ROWS > 2)
  {
    lcd.setCursor(0, 2);
    lcd.print("World !!");
  }
  if (LCD_ROWS > 3)
  {
    lcd.setCursor(0, 3);
    lcd.print("3.1415..");
  }
}

void loop()
{
  delay(1000);

  // Move cursor right
  lcd.home();
  lcd.noBlink();
  lcd.cursor();
  for (int i = 0 ; i < LCD_COLS ; i++)
  {
    lcd.setCursor(i, 0);
    delay(300);
  }
  lcd.blink();
  lcd.noCursor(); // note: blink + cursor could also be set together
  
  delay(1000);

  // Move cursor left
  for (int i = (LCD_COLS - 1) ; i >= 0 ; i--)
  {
    lcd.setCursor(i, 0);
    delay(300);
  }
}
