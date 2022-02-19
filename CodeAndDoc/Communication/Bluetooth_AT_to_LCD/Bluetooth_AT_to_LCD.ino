/*  
  Bluetooth ZS-040 (HC-05 or HC-06) module

  Attention:
  Module must not be connected to bluetooth, 
  otherwise the AT commands are not working!
  
  Level shifter:
  Remember to use a level shifter for RX and TX or at least a voltage 
  divider from the Arduino TX pin (5V) to the module RX pin (3.3V)
  (1K and 2K resistors give a good divider)

  http://www.martyncurrey.com/hc-05-and-hc-06-zs-040-bluetooth-modules-first-look/
  http://www.martyncurrey.com/hc-06-hc01-comv2-0/
*/
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C g_lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 lines display

void setup()
{
  Serial.begin(9600);
  while (!Serial);  // if you want your program to wait for Serial Monitor to be opened before running when 
                    // using native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)

  g_lcd.init();
  g_lcd.backlight();      // turn on the backlight
  g_lcd.clear();          // returns to home position and clears everything, while home() just returns to home position
  g_lcd.setCursor(0, 0);  // column, row
  g_lcd.print("Bluetooth Test  ");
  g_lcd.setCursor(0, 1);
  g_lcd.print("HC-05 or HC-06  ");

  delay(2000);
}

void loop()
{
  String str;

  g_lcd.clear();          // returns to home position and clears everything, while home() just returns to home position
  
  // AT
  g_lcd.setCursor(0, 0);
  g_lcd.print("AT              ");
  g_lcd.setCursor(0, 1);
  g_lcd.print("                ");
  Serial.println("AT");
  while (Serial.available() == 0);
  str = Serial.readString();
  str.remove(str.indexOf('\r'));
  g_lcd.setCursor(0, 1);
  g_lcd.print(str);

  delay(2000);
  
  // AT+NAME?
  g_lcd.setCursor(0, 0);
  g_lcd.print("AT+NAME?        ");
  g_lcd.setCursor(0, 1);
  g_lcd.print("                ");
  Serial.println("AT+NAME?");
  while (Serial.available() == 0);
  str = Serial.readString();
  str.remove(str.indexOf('\r'));
  g_lcd.setCursor(0, 1);
  g_lcd.print(str);

  delay(2000);
  
  // AT+UART?
  g_lcd.setCursor(0, 0);
  g_lcd.print("AT+UART?        ");
  g_lcd.setCursor(0, 1);
  g_lcd.print("                ");
  Serial.println("AT+UART?");
  while (Serial.available() == 0);
  str = Serial.readString();
  str.remove(str.indexOf('\r'));
  g_lcd.setCursor(0, 1);
  g_lcd.print(str);
  
  delay(2000);

  // AT+PSWD?
  g_lcd.setCursor(0, 0);
  g_lcd.print("AT+PSWD?        ");
  g_lcd.setCursor(0, 1);
  g_lcd.print("                ");
  Serial.println("AT+PSWD?");
  while (Serial.available() == 0);
  str = Serial.readString();
  str.remove(str.indexOf('\r'));
  g_lcd.setCursor(0, 1);
  g_lcd.print(str);
  
  delay(2000);
  
  // AT+ROLE?
  g_lcd.setCursor(0, 0);
  g_lcd.print("AT+ROLE?        ");
  g_lcd.setCursor(0, 1);
  g_lcd.print("                ");
  Serial.println("AT+ROLE?");
  while (Serial.available() == 0);
  str = Serial.readString(); // 0=slave , 1=master
  str.remove(str.indexOf('\r'));
  g_lcd.setCursor(0, 1);
  g_lcd.print(str);
  
  delay(2000);
  
  // AT+VERSION?
  g_lcd.setCursor(0, 0);
  g_lcd.print("AT+VERSION?                             ");
  g_lcd.setCursor(0, 1);
  g_lcd.print("                                        ");
  Serial.println("AT+VERSION?");
  while (Serial.available() == 0);
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

  delay(2000);
}
