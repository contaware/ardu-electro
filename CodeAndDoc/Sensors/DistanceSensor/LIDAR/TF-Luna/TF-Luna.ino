/*
  TF-Luna a LIDAR (Light Detection and Ranging) distance sensor

  - VDD supply is 3.7V - 5.2V (~70mA) and communication signal levels are 3.3V.

  - SDA and SCL require pull-ups to 3.3V. Use 10kΩ for 100kHz and 2kΩ for 400kHz.
  
  - It measures distances between 0.2m-8m and has an accuracy of 6cm in the 0.2m-3m
    range and 2% in the 3m-8m range.

  - When the sensor faces up, we have from left to right:
     1       2       3       4       5       6 
    5V    RXD/SDA TXD/SCL   GND    Mode  Data signal

    Mode pin connected to GND: I2C (default address is 0x10, see TFL_DEF_ADR)
    Mode pin disconnected or connected to 3.3V: UART (default baudrate is 115200)

    Data signal can be setup as a data ready signal or it can be setup as a on/off 
    signal to detect objects closer than a configured distance.
*/
#include <Wire.h>
#include <TFLI2C.h> // https://github.com/budryerson/TFLuna-I2C

TFLI2C tflI2C;
int16_t tflDistCm;
int16_t tflAddr = TFL_DEF_ADR;
uint8_t tflVer[3];

void setup()
{
  // Init Serial
  Serial.begin(9600);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!
  delay(5000);      // for ESP32 and some other MCUs a delay() is needed, otherwise
                    // the messages generated in setup() can't be seen!

  Serial.println("TF-Luna LIDAR Test");

  // I2C begin
  Wire.begin();
  //Wire.setClock(400000); // default is standard mode=100kHz, uncomment for fast mode=400kHz

  // Check whether device is available by asking for firmware version
  if (tflI2C.Get_Firmware_Version(tflVer, tflAddr))
  {
    Serial.print("Firmware Version: ");
    Serial.print(tflVer[2]);
    Serial.print(".");
    Serial.print(tflVer[1]);
    Serial.print(".");
    Serial.println(tflVer[0]);
  }
  else
  {
    Serial.println("ERROR: can't find TF-Luna!");
    while (true);
  }
}
 
void loop()
{  
  if (tflI2C.getData(tflDistCm, tflAddr))
  {
    Serial.print(tflDistCm);
    Serial.println("cm");
  }
  delay(50);
}
