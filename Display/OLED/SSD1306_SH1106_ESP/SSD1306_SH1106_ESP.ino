/*
  OLED with SSD1306/SH1106 driver chip connected to ESP8266/ESP32/Mbed-OS
  
  - The WeMos D1 Mini 0.66" 64x48 shields from Lolin have 2x I2C buttons 
    at I2C Address 0x31.
    https://github.com/wemos/LOLIN_OLED_I2C_Button_Library
  
  - The WeMos D1 Mini 0.66" 64x48 shield clones have 2x buttons connected
    to the D3 and D4 pins (can be changed to D5, D6 or D7).
*/
#define OLED_SHIELD_BUTTON_A        D3                        // comment if not having these buttons
#define OLED_SHIELD_BUTTON_B        D4                        // comment if not having these buttons
#define SCREEN_ADDRESS              0x3C                      // see board for address: 0x3C or 0x3D
#include "SSD1306Wire.h"                                      // #include "SSD1306Wire.h" or #include "SH1106Wire.h"
                                                              // Library name: ESP8266 and ESP32 OLED driver for SSD1306 displays
                                                              // https://github.com/ThingPulse/esp8266-oled-ssd1306
SSD1306Wire oled(SCREEN_ADDRESS, SDA, SCL, GEOMETRY_64_48);   // SSD1306Wire or SH1106Wire
                                                              // GEOMETRY_128_64, GEOMETRY_128_32, GEOMETRY_64_48, GEOMETRY_64_32
                                                            
void setup()
{
  // Init shield buttons
#if defined(OLED_SHIELD_BUTTON_A) && defined(OLED_SHIELD_BUTTON_B)
  pinMode(OLED_SHIELD_BUTTON_A, INPUT); 
  pinMode(OLED_SHIELD_BUTTON_B, INPUT);
#endif
   
  // Initialize the display
  oled.init();

  // Rotate the screen by 180°
  oled.flipScreenVertically();

  // Init text
  oled.setTextAlignment(TEXT_ALIGN_LEFT);
  oled.setFont(ArialMT_Plain_10); // ArialMT_Plain_10, ArialMT_Plain_16 or ArialMT_Plain_24

  // Text normally gets cut                                  
  oled.clear();
  oled.drawString(0, 0, "That's a mega long line");
  oled.display();
  delay(2000);

  // Text that wraps
  oled.clear();
  oled.drawStringMaxWidth(0, 0, oled.width(), "That's a mega long line");
  oled.display();
  delay(2000);

  // Text alignment
  oled.clear();
  oled.drawString(0, 0, "L-align");
  oled.setTextAlignment(TEXT_ALIGN_CENTER);
  oled.drawString(oled.width() / 2, 10, "C-align");
  oled.setTextAlignment(TEXT_ALIGN_RIGHT);
  oled.drawString(oled.width(), 20, "R-align");
  oled.display();
  delay(2000);

  // Draw
  oled.clear();
  oled.setPixel(0, 0);
  oled.setPixel(oled.width() - 1, 0);
  oled.setPixel(oled.width() - 1, oled.height() - 1);
  oled.setPixel(0, oled.height() - 1);
  oled.display();
  delay(1000);
  oled.drawRect(2, 2, oled.width() - 4, oled.height() - 4); // provide top-left edge and size
  oled.display();
  delay(1000);
  oled.drawLine(2, 2, oled.width() - 3, oled.height() - 3); // start point and end point are drawn
  oled.display();
  delay(1000);
  oled.drawLine(2, oled.height() - 3, oled.width() - 3, 2); // start point and end point are drawn
  oled.display();
  delay(1000);
  oled.fillRect(2, 2, oled.width() - 4, oled.height() - 4); // provide top-left edge and size
  oled.display();
  delay(2000);
}

void loop()
{
#if defined(OLED_SHIELD_BUTTON_A) && defined(OLED_SHIELD_BUTTON_B)
  oled.clear();
  oled.setTextAlignment(TEXT_ALIGN_CENTER);
  oled.drawString(oled.width() / 2, 0, "Press");
  oled.drawString(oled.width() / 2, 10, "button(s)");
  if (digitalRead(OLED_SHIELD_BUTTON_A) == LOW)
  {
    oled.setTextAlignment(TEXT_ALIGN_LEFT);
    oled.drawString(0, oled.height() - 12, "A");
  }
  if (digitalRead(OLED_SHIELD_BUTTON_B) == LOW)
  {
    oled.setTextAlignment(TEXT_ALIGN_RIGHT);
    oled.drawString(oled.width(), oled.height() - 12, "B");
  }
  oled.display();
  delay(100);
#endif
}
