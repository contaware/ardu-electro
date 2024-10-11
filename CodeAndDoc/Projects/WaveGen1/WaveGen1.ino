/*
  My waveform generator 1 is based on a AD9833 module, a 16x2 LCD with HD44780 chip and 
  PCF8574 I2C expander and a rotary encoder
 
  - VDD supply of the Arduino board is 7V - 12V. There is a negative voltage module for 
    the opamp and the buffer. The waveform generator works with the 5V of the USB, but 
    then the opamp positive supply is a bit low and that could distort the positive wave 
    when at maximum output level.

  - The AD9833 module outputs the Sine and the Triangle waves at 0.6Vpp, the signal gets 
    amplified by a LM318N opamp and buffered with a BUF634. We do not support the square 
    wave output because it swings the full range from ground to VDD. Note that the output
    amplitude decreases a bit with higher frequencies because of the AD9833 module itself
    and also because of the bandwidth limit of the LM318N opamp.

  - To have a nice waveform the frequency is limited to 1 MHz (both AD9833 and LM318N are
    the limiting factors).
*/
#include <AD9833.h> // download from https://github.com/Billwilliams1952/AD9833-Library-Arduino
#include "RotaryEncoderPoll.h"
#include "ButtonPoll.h"
#include <LiquidCrystal_I2C.h>

const long MAX_FREQ = 1000000;      // maximum adjustable frequency
long freqHz = 0;                    // init frequency from 0 to MAX_FREQ
WaveformType waveType = SINE_WAVE;  // init wave form to SINE_WAVE or TRIANGLE_WAVE
const byte AD9833_FNC_PIN = 10;     // can be any digital IO pin
AD9833 gen(AD9833_FNC_PIN);         // defaults to 25 MHz internal reference frequency

RotaryEncoderPoll enc;
ButtonPoll btn;
unsigned int pos = 3;               // cursor position: 0..7
bool encLock = false;
unsigned long lastBtnPressMs;
const unsigned long BTN_PRESS_TIMEOUT_MS = 350;
const byte ROTARY_ENCODER_CLK_PIN = 8;
const byte ROTARY_ENCODER_DT_PIN = 9;
const byte ROTARY_ENCODER_SW_PIN = 7;

const uint8_t LCD_COLS = 16;
const uint8_t LCD_ROWS = 2;
LiquidCrystal_I2C lcd(0x27, LCD_COLS, LCD_ROWS); // PCF8574: 0x20 - 0x27 (default), PCF8574A: 0x38 - 0x3F (default)
const char arrow_up_char[] = {0b00100, 0b01110, 0b11111, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100};  // custom char (5 x 8 dots)

void setup()
{   
  // Init wave form generator module
  gen.Begin();  // this MUST be the first command after declaring the AD9833 object
  gen.ApplySignal(waveType, REG0, freqHz);
  if (freqHz > 0)
    gen.EnableOutput(true); // by default the output is disabled

  // Init rotary encoder with button
  enc.begin(ROTARY_ENCODER_CLK_PIN, ROTARY_ENCODER_DT_PIN);
  btn.begin(ROTARY_ENCODER_SW_PIN);
    
  // Init LCD module
  lcd.init();
  lcd.backlight(); // turn on the backlight
  lcd.createChar(0, arrow_up_char);
  lcd.clear();
  lcdUpdate();
}

void lcdUpdate()
{
  // First row
  lcd.setCursor(0, 0);
  if (waveType == SINE_WAVE)
    lcd.print("Sine");                                          // this must be 4 chars
  else
    lcd.print("Tria");                                          // this must be 4 chars
  char s[13];
  char sFreq[8];
  dtostrf(gen.GetActualProgrammedFrequency(REG0), 7, 0, sFreq); // this is exactly 7 chars
  sprintf(s, "  %s Hz", sFreq);                                 // this is exactly 12 chars
  lcd.print(s);

  // Second row
  lcd.setCursor(0, 1);
  lcd.print("_     _______   ");                                // this are 16 chars
  if (pos <= 6)
  {
    lcd.setCursor(12 - pos, 1);
    lcd.write(0); // up-arrow
  }
  else
  {
    lcd.setCursor(0, 1);
    lcd.write(0); // up-arrow
  }
}

long pow10(unsigned int exponent)
{
  switch (exponent)
  {
    case 0: return 1;
    case 1: return 10;
    case 2: return 100;
    case 3: return 1000;
    case 4: return 10000;
    case 5: return 100000;
    case 6: return 1000000;
    default: return 0;
  }
}

void loop()
{
  // If BTN_PRESS_TIMEOUT_MS elapsed from the last button press 
  // then unlock the rotary encoder
  if (encLock && (millis() - lastBtnPressMs > BTN_PRESS_TIMEOUT_MS))
      encLock = false;
      
  // Read rotary encoder rotation
  int res = enc.read(); // also if encLock is set continue to read the encoder to correctly debounce it!
  if (!encLock)
  {
    if (res == 1)
    {
      if (pos <= 6)
      {
        long newFreqHz = freqHz - pow10(pos);
        if (newFreqHz >= 0)
        {
          gen.SetFrequency(REG0, freqHz = newFreqHz);
          gen.EnableOutput(freqHz > 0);
        }
      }
      else
      {
        if (waveType == SINE_WAVE)
          gen.SetWaveform(REG0, waveType = TRIANGLE_WAVE);
        else
          gen.SetWaveform(REG0, waveType = SINE_WAVE);
      }
  
      // Update the display
      lcdUpdate();
    }
    else if (res == -1)
    {
      if (pos <= 6)
      {
        long newFreqHz = freqHz + pow10(pos);
        if (newFreqHz <= MAX_FREQ)
        {
          gen.SetFrequency(REG0, freqHz = newFreqHz);
          gen.EnableOutput(freqHz > 0);
        }
      }
      else
      {
        if (waveType == SINE_WAVE)
          gen.SetWaveform(REG0, waveType = TRIANGLE_WAVE);
        else
          gen.SetWaveform(REG0, waveType = SINE_WAVE);
      }
  
      // Update the display
      lcdUpdate();
    }
  }

  // On rotary encoder's button press
  if (btn.pressed())
  {
    // Go to next position
    pos = (++pos) % 8;  // 0..7

    // Lock the rotary encoder for BTN_PRESS_TIMEOUT_MS
    encLock = true;
    lastBtnPressMs = millis();
    
    // Update the display
    lcdUpdate();
  }
}
