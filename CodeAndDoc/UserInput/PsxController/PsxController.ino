/*
  Wireless Playstation Controller
  
  - VDD supply is 3.3V.
  
  - It's important to use a level shifter from the Arduino 5V to the 
    controller 3.3V for all four pins (5V do work, but it can damage 
    the controller).

  - When driving the controller through the hardware SPI port, the pins
    are fixed and depend on the board/microcontroller being used. For 
    instance, on an Arduino Uno the connections must be as follows:
    CMD  <-> UNO=11 (MOSI)
    DATA <-> UNO=12 (MISO)
    CLK  <-> UNO=13 (SCK)
    ATT  <-> UNO=10 (SS)
    Note: any pin can be used for ATT, but please note that most 8-bit
          AVRs require the HW SPI SS pin to be kept as an output for 
          HW SPI to be in main mode.
*/
#include <DigitalIO.h>

#define USE_BITBANG      true

#if USE_BITBANG == true
#include <PsxControllerBitBang.h> // PsxNewLib by SukkoPera
                                  // https://github.com/SukkoPera/PsxNewLib
const byte PSX_CMD_PIN = 11;
const byte PSX_DAT_PIN = 12;
const byte PSX_CLK_PIN = 13;
const byte PSX_ATT_PIN = 10;
PsxControllerBitBang<PSX_ATT_PIN, PSX_CMD_PIN, PSX_DAT_PIN, PSX_CLK_PIN> psx;
#else
#include <PsxControllerHwSpi.h>   // PsxNewLib by SukkoPera
                                  // https://github.com/SukkoPera/PsxNewLib
const byte PSX_ATT_PIN = 10;
PsxControllerHwSpi<PSX_ATT_PIN> psx;
#endif

bool haveController = false;

void setup()
{
  // Init Serial
  Serial.begin(9600);

  // Give controller some time
  delay(1000);
}

void loop()
{
  if (!haveController)
  {
    if (psx.begin())
    {
      Serial.print("Controller found ");
#if USE_BITBANG == true
      Serial.println("(bit-bang communication)");
#else
      Serial.println("(hw SPI communication)");
#endif
      delay(1000); // give controller some time
      if (!psx.enterConfigMode())
        Serial.println("Cannot enter config mode");
      else
      {
        PsxControllerType ctype = psx.getControllerType();
        switch (ctype)
        {
          case PSCTRL_UNKNOWN:    Serial.println("Unknown"); break;
          case PSCTRL_DUALSHOCK:  Serial.println("Dual Shock"); break;
          case PSCTRL_DSWIRELESS: Serial.println("Dual Shock Wireless"); break;
          case PSCTRL_GUITHERO:   Serial.println("Guitar Hero"); break;
        }
        if (!psx.enableAnalogSticks())
          Serial.println("Cannot enable analog sticks");
        if (!psx.enableAnalogButtons())
          Serial.println("Cannot enable analog buttons");
        if (!psx.exitConfigMode())
          Serial.println("Cannot exit config mode");
      }
      haveController = true;
    }
  }
  else
  {
    if (!psx.read())
    {
      Serial.println("Controller lost!");
      haveController = false;
    }
    else
    {
      uint16_t btns = psx.getButtonWord();
      if (PSB_SELECT & btns)    Serial.println("SEL");
      if (PSB_L3 & btns)        Serial.println("L3");
      if (PSB_R3 & btns)        Serial.println("R3");
      if (PSB_START & btns)     Serial.println("START");
      if (PSB_PAD_UP & btns)    Serial.println("^");
      if (PSB_PAD_RIGHT & btns) Serial.println(">");
      if (PSB_PAD_DOWN & btns)  Serial.println("v");
      if (PSB_PAD_LEFT & btns)  Serial.println("<");
      if (PSB_L2 & btns)        Serial.println("L2");
      if (PSB_R2 & btns)        Serial.println("R2");
      if (PSB_L1 & btns)        Serial.println("L1");
      if (PSB_R1 & btns)        Serial.println("R1");
      if (PSB_TRIANGLE & btns)  Serial.println("TRIANGLE");
      if (PSB_CIRCLE & btns)    Serial.println("CIRCLE");
      if (PSB_CROSS & btns)     Serial.println("CROSS");
      if (PSB_SQUARE & btns)    Serial.println("SQUARE");

      byte lx, ly, rx, ry;
      psx.getLeftAnalog(lx, ly);
      psx.getRightAnalog(rx, ry);
      if (lx != 128 || ly != 128 || rx != 128 || ry != 128)
      {
        Serial.print("L="); Serial.print(lx); Serial.print(","); Serial.print(ly); 
        Serial.print(" R=");Serial.print(rx); Serial.print(","); Serial.println(ry);
      }
    }
  }
  delay(100);
}
