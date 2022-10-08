/*
  VL53L0X distance sensor module
  
  - VDD supply is 2.6V - 5.5V.
  
  - The violet VL53L0XV2 module has a 2.8V regulator and integrated level-shifters.
*/
#include "Adafruit_VL53L0X.h"
 
Adafruit_VL53L0X lox = Adafruit_VL53L0X();
 
void setup()
{
  // Serial Debug
  Serial.begin(115200);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!

  // Init VL53L0X
  Serial.println(F("VL53L0X Test"));
  if (!lox.begin()) // usually address is 0x29, to pass another one use lox.begin(0x30)
  {
    Serial.println(F("Failed to boot VL53L0X"));
    while (true);
  }
  Serial.println();
}

void loop()
{
  VL53L0X_RangingMeasurementData_t measure;
 
  Serial.print(F("Starting measurement... "));
  unsigned long startMs = millis();
  lox.getSingleRangingMeasurement(&measure, false); // this is blocking; pass in 'true' to get debug data printout
  unsigned long endMs = millis();
  Serial.print(F("done in "));
  Serial.print(endMs - startMs);
  Serial.println(F("ms"));

  Serial.print(F("Distance: "));
  if (measure.RangeStatus != 4) // phase failures have incorrect data
  {
    Serial.print(measure.RangeMilliMeter);
    Serial.println(F("mm"));
  }
  else
    Serial.println(F("out of range"));

  Serial.println();
  
  delay(500);
}
