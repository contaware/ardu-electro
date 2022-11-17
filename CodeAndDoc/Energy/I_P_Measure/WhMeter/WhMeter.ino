/*
  Watt x hour meter with voltage divider, WCS1800 current sensor and LCD 1602 over I2C
 
  - VDD supply for Arduino Nano from 12V battery.

  - Voltage divider 5x.
  
  - WCS1800 with 66mV/A and max 35A.

  - We sample as fast as possible because the current may oscillate 
    (for example the tested APC UPS varies the current at 100Hz).
*/
#include <LiquidCrystal_I2C.h>

// LCD
const uint8_t LCD_COLS = 16;
const uint8_t LCD_ROWS = 2;
LiquidCrystal_I2C lcd(0x27, LCD_COLS, LCD_ROWS); // PCF8574: 0x20 - 0x27 (default), PCF8574A: 0x38 - 0x3F (default)
char lcdLine1[16]; // not null terminated
char lcdLine2[16]; // not null terminated
int lcdLine1Pos = -1;
int lcdLine2Pos = -1;

// Button
const byte BUTTON_PIN = 7;
unsigned int btnPressedCount = 0;

// Voltage
const byte DIVIDER_PIN = A1;
const float factorVoltage = 5.0 * 5.0 / 1023;
float avgVoltage = 0.0;

// Current
const byte WCS1800_PIN = A0;
int zeroCurrent = 533; // value measured for the used sensor, usually around 512, but sometimes far off...
const float factorCurrent = 5.0 / (1023.0 * 0.066);
float avgCurrent = 0.0;
float prevInstCurrent = 0.0;
int64_t ampMicros = 0;

// Power
float prevInstPower = 0.0;
int64_t wattMicros = 0;

// Time
unsigned long prevFastMicros;
unsigned long prevSlowMicros;

// Calibrate
// Note: to zeroCalibrate() an APC, turn it OFF but also unplug it from 
//       the AC line otherwise it keeps charging/maintaining the battery.
#define DO_CALIBRATE_CURRENT_SENSOR       false
#if DO_CALIBRATE_CURRENT_SENSOR == true
const int CALIB_SIZE = 250;
static void zeroCalibrate()
{
  analogRead(WCS1800_PIN);  // read analog pin to stabilize the ADC
  uint32_t startMicros;
  int32_t dataSum = 0;
  for (int i = 0; i < CALIB_SIZE; i++)
  {
    startMicros = micros();
    dataSum += analogRead(WCS1800_PIN);   // AVR takes ~100us to read it
    while (micros() - startMicros < 829); // sample rate = 1200Hz
  }
  zeroCurrent = dataSum / CALIB_SIZE;
}
#endif

void setup()
{
  // Calibrate
#if DO_CALIBRATE_CURRENT_SENSOR == true
  Serial.begin(9600);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!
  zeroCalibrate();
  Serial.print("zeroCurrent: ");
  Serial.println(zeroCurrent);
#endif

  // LCD
  lcd.init();             // this calls Wire.begin() which sets the default clock of 100000 (100kHz)
  Wire.setClock(400000);  // use fast mode at 400000 (400kHz)
  lcd.backlight();        // turn on the backlight
  lcd.clear();            // return to home position and clear everything

  // Button
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // Time
  unsigned long currentMicros = micros();
  prevFastMicros = currentMicros;
  prevSlowMicros = currentMicros;
}
  
void loop()
{ 
  // Current time
  unsigned long currentMicros = micros();

  // Sample voltage & current and do a single LCD operation
  // Note: cannot sample faster than 1000Hz because the code here can take up to 1ms
  if (currentMicros - prevFastMicros > 1200)
  {
    // Instantaneous
    int rawVoltage = analogRead(DIVIDER_PIN); // AVR takes ~100us to read it
    int rawCurrent = analogRead(WCS1800_PIN); // AVR takes ~100us to read it
    float instVoltage = rawVoltage * factorVoltage;
    float instCurrent = (rawCurrent - zeroCurrent) * factorCurrent;
    float instPower = instVoltage * instCurrent;
    ampMicros += (int64_t)((prevInstCurrent + instCurrent) / 2.0 * (float)(currentMicros - prevFastMicros)); // trapezoidal integration
    wattMicros += (int64_t)((prevInstPower + instPower) / 2.0 * (float)(currentMicros - prevFastMicros)); // trapezoidal integration
    
    // Moving average
    avgVoltage = (15 * avgVoltage + instVoltage) / 16;
    avgCurrent = (127 * avgCurrent + instCurrent) / 128;
    
    // One I2C operation at the time (I2C @ 400kHz takes ~650us for each char write or position change)
    if (lcdLine1Pos == 16)
    {
      lcd.setCursor(0, 1);  // change to second line
      lcdLine1Pos = -1;     // disable writing of first line
      lcdLine2Pos = 0;      // start writing second line
    }
    else if (lcdLine2Pos == 16)
    {
      lcd.setCursor(0, 0);  // change back to first line
      lcdLine1Pos = -1;     // disable writing of first line
      lcdLine2Pos = -1;     // disable writing of second line
    }
    else if (lcdLine1Pos >= 0 && lcdLine1Pos <= 15)
      lcd.write(lcdLine1[lcdLine1Pos++]);
    else if (lcdLine2Pos >= 0 && lcdLine2Pos <= 15)
      lcd.write(lcdLine2[lcdLine2Pos++]);

    // Update vars for the next loop round
    prevInstCurrent = instCurrent;
    prevInstPower = instPower;
    prevFastMicros = currentMicros;
  }

  // Handle button press and update LCD buffers
  // Note: to make sure that all chars are displayed on the LCD,
  //       this must be executed at least 34x slower than the above
  if (currentMicros - prevSlowMicros > 300000)
  {
    // Long press (~3 sec) to reset Ah and Wh
    bool displayAhWh = true;
    if (digitalRead(BUTTON_PIN) == HIGH)
    {
      ++btnPressedCount;
      if (btnPressedCount == 5 || btnPressedCount == 7 || btnPressedCount == 9)
        displayAhWh = false; // flash display before reset
      if (btnPressedCount >= 10)
      {
        ampMicros = 0;
        wattMicros = 0;
        btnPressedCount = 0;
      }
    }
    else
      btnPressedCount = 0;

    // Update LCD buffers
    if (lcdLine1Pos == -1 && lcdLine2Pos == -1)
    {
      // Init bufs
      memset(lcdLine1, ' ', 16);
      memset(lcdLine2, ' ', 16);
      char buf[7]; // the below dtostrf() write 6 chars + null char

      // Convert to Ah and Wh
      float Ah = (float)(ampMicros / 1000000) / 3600.0;
      float AhAbs = fabs(Ah);
      float Wh = (float)(wattMicros / 1000000) / 3600.0;
      float WhAbs = fabs(Wh);
      if (AhAbs > 99999.0 || WhAbs > 99999.0) // do reset on display overflow 
      {
        ampMicros = 0;
        wattMicros = 0;
        Ah = 0.0;
        AhAbs = 0.0;
        Wh = 0.0;
        WhAbs = 0.0;
      }

      // Ah
      // Note: to make sure that dtostrf() does not round to one digit more,
      //       do <= 9.0, 99.0 or 999.0 and not < 10.0, 100.0 or 1000.0
      if (displayAhWh)
      {
        if (AhAbs <= 9.0)
          dtostrf(Ah, 6, 3, buf);
        else if (AhAbs <= 99.0)
          dtostrf(Ah, 6, 2, buf);
        else if (AhAbs <= 999.0)
          dtostrf(Ah, 6, 1, buf);
        else
          dtostrf(Ah, 6, 0, buf);
        memcpy(lcdLine1, buf, 6);
        lcdLine1[6] = 'A';
        lcdLine1[7] = 'h';
      }
      
      // A
      dtostrf(avgCurrent, 6, 1, buf);
      memcpy(lcdLine1 + 9, buf, 6);
      lcdLine1[15] = 'A';

      // Wh
      // Note: to make sure that dtostrf() does not round to one digit more,
      //       do <= 9.0, 99.0 or 999.0 and not < 10.0, 100.0 or 1000.0
      if (displayAhWh)
      {
        if (WhAbs <= 9.0)
          dtostrf(Wh, 6, 3, buf);
        else if (WhAbs <= 99.0)
          dtostrf(Wh, 6, 2, buf);
        else if (WhAbs <= 999.0)
          dtostrf(Wh, 6, 1, buf);
        else
          dtostrf(Wh, 6, 0, buf);
        memcpy(lcdLine2, buf, 6);
        lcdLine2[6] = 'W';
        lcdLine2[7] = 'h';
      }
      
      // V
      dtostrf(avgVoltage, 6, 1, buf);
      memcpy(lcdLine2 + 9, buf, 6);
      lcdLine2[15] = 'V';

      // Start writing first line
      lcdLine1Pos = 0;
    }

    // Update var for the next loop round
    prevSlowMicros = currentMicros;
  }
}
