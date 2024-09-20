/*
  TCA9548A 8-channel bidirectional I2C switch

  - VCC supply is 1.65V - 5.5V.

  - Supports 0-400kHz clock frequency.

  - Can also be used for voltage translation by using pull-up resistors to the 
    desired voltage for the Controller and for each Target channel.

  - Especially useful when there is a need to control multiple I2C devices with
    the same I2C address.
*/
#include <Wire.h>

// I2C address selected through A0, A1, A2 pins, ranges from 0x70-0x77
const uint8_t TCA9548A_ADDR = 0x70;

// I2C address of device to test
const uint8_t I2C_ADDR = 0x76;

// Select a single channel from 0..7, for other values disable all channels.
// Notes: 1. When TCA9548A starts, or after a reset, no channel is selected.
//        2. TCA9548A can also select multiple channels (not implemented here).
void channelSel(int ch)
{
  if (ch >= 0 && ch <= 7)
  {
    Serial.print("Selected channel: "); Serial.println(ch);
    Wire.beginTransmission(TCA9548A_ADDR);
    Wire.write(1 << ch);
    Wire.endTransmission();
  }
  else
  {
    Serial.println("Disabled all channels!"); 
    Wire.beginTransmission(TCA9548A_ADDR);
    Wire.write(0);
    Wire.endTransmission();
  }
}

void setup()
{
  // Init Serial
  Serial.begin(9600);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!
  delay(5000);      // for ESP32 and some other MCUs a delay() is needed, otherwise
                    // the messages generated in setup() can't be seen!

  Serial.println("TCA9548A Test: type channel in upper window and press ENTER");

  // I2C begin
  Wire.begin();
  //Wire.setClock(400000); // default is standard mode=100kHz, uncomment for fast mode=400kHz 
}

void loop()
{
  // Get data from serial monitor's input field and select wanted channel
  if (Serial.available())
  {
    String msg;
    msg = Serial.readStringUntil('\n'); // function removes '\n' from serial buffer and does not return a '\n'
    msg.trim();                         // remove CR if terminal is sending one
    channelSel(msg.toInt());
  }

  // Search for given I2C device
  Wire.beginTransmission(I2C_ADDR);
  byte ret = Wire.endTransmission();
  if (ret == 0)
  {
    Serial.print("Found address: ");
    Serial.print("0x"); Serial.println(I2C_ADDR, HEX);
  }
  else
  {
    Serial.print("NOT found address: ");
    Serial.print("0x"); Serial.print(I2C_ADDR, HEX);
    Serial.print(", error="); Serial.println(ret);
  }
    
  // Wait a moment
  delay(3000);
}
