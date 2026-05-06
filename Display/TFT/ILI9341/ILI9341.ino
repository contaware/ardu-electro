/*
  Adafruit & cloned 2.8"/3.2" TFT with ILI9341 driver chip

  - The display has a resolution of 240x320 and uses the 16-bit 
    RGB565 color format.
  
  - The chip supports an 8-bit parallel interface or SPI. Here we 
    show the SPI mode, for the Adafruit module solder the jumpers 
    indicated IM1, IM2 and IM3.

  - The ILI9341 chip requires 3.3V supply and 3.3V logic levels for 
    communication.
  
  - Adafruit modules have both a 3.3V LDO and 74LVX245 level shifters 
    so that they are 3.3V and 5V compatible. 
    
  - Cloned modules usually have the 3.3V LDO but no level shifters.

  - Connect the following pins:
    MISO <-> MISO of your board (not necessary for the display)
    MOSI <-> MOSI of your board
    CLK <-> SCK of your board
    CS <-> see TFT_CS define
    D/C (data/command selector pin) <-> see TFT_DC define
    RST (reset pin) <-> see TFT_RST define
    LED (Backlight LED) <-> connect to 3.3V
*/
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

#define TFT_CS   10
#define TFT_DC    8
#define TFT_RST   9
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
 
void setup()
{
  // Initialize display
  tft.begin();

  // Set display rotation (0-3)
  tft.setRotation(0);
}

void loop()
{
  // Fill screen with black
  tft.fillScreen(ILI9341_BLACK);

  // Set text size, multiplier 1-6
  tft.setTextSize(2);

  // Draw text
  tft.setCursor(0, 0); // start at top-left corner
  tft.setTextColor(ILI9341_GREEN);
  tft.println("Wrapping is ON by default.");
  tft.setTextWrap(false);
  tft.setTextColor(ILI9341_RED);
  tft.println("Wrapping is OFF, line cut...");
  tft.setTextWrap(true);
  tft.setTextColor(ILI9341_YELLOW);
  tft.print("0x");
  tft.println(0xDEADBEEF, HEX);
  tft.setTextColor(ILI9341_ORANGE);
  tft.println(3.141592); // only 2 decimals are printed
  delay(500);

  // Draw a filled rectangle
  int16_t posY = tft.getCursorY() + 10;
  tft.fillRect(0, posY, 49, 49, ILI9341_BLUE);
  delay(500);

  // Draw a circle
  posY += 24;
  tft.drawCircle(24, posY, 24, ILI9341_CYAN);
  delay(500);

  // Draw a line
  posY += 27;
  tft.drawLine(0, posY, 48, posY, ILI9341_BLUE);
  delay(4000);

  // Draw 4 corners & center text with a custom color
  tft.fillScreen(ILI9341_BLACK);
  tft.drawPixel(0, 0, ILI9341_WHITE);
  tft.drawPixel(tft.width() - 1, 0, ILI9341_WHITE);
  tft.drawPixel(0, tft.height() - 1, ILI9341_WHITE);
  tft.drawPixel(tft.width() - 1, tft.height() - 1, ILI9341_WHITE);
  uint16_t customColor = tft.color565(174, 235, 4); // RGB values 0-255
  tft.setTextColor(customColor);
  int16_t x1, y1;
  uint16_t w, h;
  const char centerStr[] = "CENTER"; 
  tft.getTextBounds(centerStr, 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((tft.width() - w) / 2, (tft.height() - h) / 2);
  tft.println(centerStr);
  delay(3000);

  // Draw all supported chars
  if (tft.width() > tft.height())
    tft.setTextSize(1);
  tft.cp437(false); // restore the default
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(0, 0);
  for (int i = 0; i < 256;)
  {
    if (i == '\n' || i == '\r')
      tft.write(' ');
    else           
      tft.write((uint8_t)i);
    if (++i%16 == 0)
      tft.write('\n');
  }
  delay(4000);
  tft.cp437(true); // use the 'Code Page 437'-compatible charset
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(0, 0);
  for (int i = 0; i < 256;)
  {
    if (i == '\n' || i == '\r')
      tft.write(' ');
    else           
      tft.write((uint8_t)i);
    if (++i%16 == 0)
      tft.write('\n');
  }
  delay(4000);
}
