/*  
  Bluetooth ZS-040 (HC-05 or HC-06) module (default PIN: 1234, default baud rate: 9600)

  - The HC-06 module is in AT mode when not connected/paired.
  - The HC-05 module is not in AT mode when powered-up. It has two AT modes which can be 
    entered:
    1. Power-on with the small pushbutton held in, the LEDs will now slowly blink every
       2 seconds, the baud rate is 38400. Continue to hold down the button otherwise it
       exits this mode.
    2. Power-on and then hold down the small pushbutton, the LEDs blink fast, the baud 
       rate is at the configured value which by default is 9600. Continue to hold down 
       the button otherwise it exits this mode.
  - When setting the name pay attention that your phone is not scanning, otherwise it may
    fail. To see the new name power cycle the module and sometimes also the phone.
  - Some HC-06 modules implement the HC-05 AT command set instead of the HC-06 one
    (those modules probably use the chip from the HC-05 modules configured to start in
    AT mode).
  
  Level shifter:
  Remember to use a level shifter for RX and TX or at least a voltage 
  divider from the Arduino TX pin (5V) to the module RX pin (3.3V)
  (1K and 2K resistors give a good divider).

  References:
  http://www.martyncurrey.com/hc-05-and-hc-06-zs-040-bluetooth-modules-first-look/
  http://www.martyncurrey.com/arduino-with-hc-05-bluetooth-module-at-mode/
  http://www.martyncurrey.com/hc-06-hc01-comv2-0/
*/
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C g_lcd(0x27, 16, 2);       // set the LCD address to 0x27 for a 16 chars and 2 lines display

#define AT_COMMANDS_STYLE           5       // 5: HC-05 or 6: HC-06
#define SERIAL_SPEED                9600    // 9600 or 38400
#define DELAY_BETWEEN_COMMANDS_MS   2000    // ms
#define AT_ANSWER_TIMEOUT_MS        4000    // ms
#define AT_ANSWER_POLL_MS           10      // ms

bool ATAnswer()
{
  unsigned long startMs = millis();
  while (Serial.available() == 0)
  {
    if (millis() - startMs > AT_ANSWER_TIMEOUT_MS)
    {
      g_lcd.setCursor(0, 1);
      g_lcd.print("no answer...");
      return false;
    }
    else
      delay(AT_ANSWER_POLL_MS);
  }
  return true;
}

void queryHC05()
{
  String str;

  g_lcd.clear();          // returns to home position and clears everything, while home() just returns to home position
  g_lcd.setCursor(0, 0);  // column, row
  g_lcd.print("Bluetooth Test  ");
  g_lcd.setCursor(0, 1);
  g_lcd.print("for HC-05       ");
  delay(DELAY_BETWEEN_COMMANDS_MS);

  // AT
  g_lcd.setCursor(0, 0);
  g_lcd.print("AT              ");
  g_lcd.setCursor(0, 1);
  g_lcd.print("                ");
  Serial.println("AT"); // println sends the necessary \r\n for us
  if (ATAnswer())
  {
    str = Serial.readString();
    str.remove(str.indexOf('\r'));
    g_lcd.setCursor(0, 1);
    g_lcd.print(str);
  }
  delay(DELAY_BETWEEN_COMMANDS_MS);

  // AT+VERSION?
  g_lcd.setCursor(0, 0);
  g_lcd.print("AT+VERSION?                             ");
  g_lcd.setCursor(0, 1);
  g_lcd.print("                                        ");
  Serial.println("AT+VERSION?"); // println sends the necessary \r\n for us
  if (ATAnswer())
  {
    str = Serial.readString();
    str.remove(str.indexOf('\r'));
    g_lcd.setCursor(0, 1);
    g_lcd.print(str);
    for (int i = 0 ; i < 10 ; i++)
    {
      g_lcd.scrollDisplayLeft();
      delay(400);
    }
    for (int i = 0 ; i < 10 ; i++)
    {
      g_lcd.scrollDisplayRight();
      delay(400);
    }
  }
  delay(DELAY_BETWEEN_COMMANDS_MS);
  
  // AT+NAME?
  g_lcd.setCursor(0, 0);
  g_lcd.print("AT+NAME?        ");
  g_lcd.setCursor(0, 1);
  g_lcd.print("                ");
  Serial.println("AT+NAME?"); // println sends the necessary \r\n for us
  if (ATAnswer())
  {
    str = Serial.readString();
    str.remove(str.indexOf('\r'));
    g_lcd.setCursor(0, 1);
    g_lcd.print(str);
  }
  delay(DELAY_BETWEEN_COMMANDS_MS);

  // AT+UART?
  g_lcd.setCursor(0, 0);
  g_lcd.print("AT+UART?        ");
  g_lcd.setCursor(0, 1);
  g_lcd.print("                ");
  Serial.println("AT+UART?"); // println sends the necessary \r\n for us
  if (ATAnswer())
  {
    str = Serial.readString();
    str.remove(str.indexOf('\r'));
    g_lcd.setCursor(0, 1);
    g_lcd.print(str);
  }
  delay(DELAY_BETWEEN_COMMANDS_MS);

  // AT+PSWD?
  g_lcd.setCursor(0, 0);
  g_lcd.print("AT+PSWD?        ");
  g_lcd.setCursor(0, 1);
  g_lcd.print("                ");
  Serial.println("AT+PSWD?"); // println sends the necessary \r\n for us
  if (ATAnswer())
  {
    str = Serial.readString();
    str.remove(str.indexOf('\r'));
    g_lcd.setCursor(0, 1);
    g_lcd.print(str);
  }
  delay(DELAY_BETWEEN_COMMANDS_MS);
  
  // AT+ROLE?
  g_lcd.setCursor(0, 0);
  g_lcd.print("AT+ROLE?        ");
  g_lcd.setCursor(0, 1);
  g_lcd.print("                ");
  Serial.println("AT+ROLE?"); // println sends the necessary \r\n for us
  if (ATAnswer())
  {
    str = Serial.readString(); // 0=slave , 1=master
    str.remove(str.indexOf('\r'));
    g_lcd.setCursor(0, 1);
    g_lcd.print(str);
  }
  delay(DELAY_BETWEEN_COMMANDS_MS);
}

void queryHC06()
{
  String str;

  g_lcd.clear();          // returns to home position and clears everything, while home() just returns to home position
  g_lcd.setCursor(0, 0);  // column, row
  g_lcd.print("Bluetooth Test  ");
  g_lcd.setCursor(0, 1);
  g_lcd.print("for HC-06       ");
  delay(DELAY_BETWEEN_COMMANDS_MS);

  // AT
  g_lcd.setCursor(0, 0);
  g_lcd.print("AT              ");
  g_lcd.setCursor(0, 1);
  g_lcd.print("                ");
  Serial.print("AT"); // no termination characters, it reacts after one second
  if (ATAnswer())
  {
    str = Serial.readString();
    str.remove(str.indexOf('\r'));
    g_lcd.setCursor(0, 1);
    g_lcd.print(str);
  }
  delay(DELAY_BETWEEN_COMMANDS_MS);

  // AT+VERSION
  g_lcd.setCursor(0, 0);
  g_lcd.print("AT+VERSION                              ");
  g_lcd.setCursor(0, 1);
  g_lcd.print("                                        ");
  Serial.print("AT+VERSION"); // no termination characters, it reacts after one second
  if (ATAnswer())
  {
    str = Serial.readString();
    str.remove(str.indexOf('\r'));
    g_lcd.setCursor(0, 1);
    g_lcd.print(str);
    for (int i = 0 ; i < 10 ; i++)
    {
      g_lcd.scrollDisplayLeft();
      delay(400);
    }
    for (int i = 0 ; i < 10 ; i++)
    {
      g_lcd.scrollDisplayRight();
      delay(400);
    }
  }
  delay(DELAY_BETWEEN_COMMANDS_MS);

  // Note: cannot query name, speed or PIN, only set them!
}

void changeHC05()
{
  String str;

  // AT+NAME
  g_lcd.setCursor(0, 0);
  g_lcd.print("AT+NAME=HC-05   ");
  g_lcd.setCursor(0, 1);
  g_lcd.print("                ");
  Serial.println("AT+NAME=HC-05"); // println sends the necessary \r\n for us
  if (ATAnswer())
  {
    str = Serial.readString();
    str.remove(str.indexOf('\r'));
    g_lcd.setCursor(0, 1);
    g_lcd.print(str);
  }
  delay(DELAY_BETWEEN_COMMANDS_MS);

  // AT+UART
  g_lcd.setCursor(0, 0);
  g_lcd.print("AT+UART=9600,0,0");
  g_lcd.setCursor(0, 1);
  g_lcd.print("                ");
  Serial.println("AT+UART=9600,0,0"); // println sends the necessary \r\n for us
  if (ATAnswer())
  {
    str = Serial.readString();
    str.remove(str.indexOf('\r'));
    g_lcd.setCursor(0, 1);
    g_lcd.print(str);
  }
  delay(DELAY_BETWEEN_COMMANDS_MS);

  // AT+PSWD
  g_lcd.setCursor(0, 0);
  g_lcd.print("AT+PSWD=1234    ");
  g_lcd.setCursor(0, 1);
  g_lcd.print("                ");
  Serial.println("AT+PSWD=1234"); // println sends the necessary \r\n for us
  if (ATAnswer())
  {
    str = Serial.readString();
    str.remove(str.indexOf('\r'));
    g_lcd.setCursor(0, 1);
    g_lcd.print(str);
  }
  delay(DELAY_BETWEEN_COMMANDS_MS);
}

void changeHC06()
{
  String str;

  // AT+NAME
  g_lcd.setCursor(0, 0);
  g_lcd.print("AT+NAMEHC-06    ");
  g_lcd.setCursor(0, 1);
  g_lcd.print("                ");
  Serial.print("AT+NAMEHC-06"); // no termination characters, it reacts after one second
  if (ATAnswer())
  {
    str = Serial.readString();
    str.remove(str.indexOf('\r'));
    g_lcd.setCursor(0, 1);
    g_lcd.print(str);
  }
  delay(DELAY_BETWEEN_COMMANDS_MS);

  // AT+BAUD
  // 1: 1200, 2: 2400, 3: 4800, 4: 9600, 5: 19200, 6: 38400, 7: 57600, 8: 115200
  g_lcd.setCursor(0, 0);
  g_lcd.print("AT+BAUD4        ");
  g_lcd.setCursor(0, 1);
  g_lcd.print("                ");
  Serial.print("AT+BAUD4"); // no termination characters, it reacts after one second
  if (ATAnswer())
  {
    str = Serial.readString();
    str.remove(str.indexOf('\r'));
    g_lcd.setCursor(0, 1);
    g_lcd.print(str);
  }
  delay(DELAY_BETWEEN_COMMANDS_MS);

  // AT+PIN
  g_lcd.setCursor(0, 0);
  g_lcd.print("AT+PIN1234      ");
  g_lcd.setCursor(0, 1);
  g_lcd.print("                ");
  Serial.print("AT+PIN1234"); // no termination characters, it reacts after one second
  if (ATAnswer())
  {
    str = Serial.readString();
    str.remove(str.indexOf('\r'));
    g_lcd.setCursor(0, 1);
    g_lcd.print(str);
  }
  delay(DELAY_BETWEEN_COMMANDS_MS);
}

void setup()
{
  Serial.begin(SERIAL_SPEED);
  
  g_lcd.init();
  g_lcd.backlight();      // turn on the backlight

#if AT_COMMANDS_STYLE == 5
  //changeHC05();
#else
  //changeHC06();
#endif
}

void loop()
{
#if AT_COMMANDS_STYLE == 5
  queryHC05();
#else
  queryHC06();
#endif
}
