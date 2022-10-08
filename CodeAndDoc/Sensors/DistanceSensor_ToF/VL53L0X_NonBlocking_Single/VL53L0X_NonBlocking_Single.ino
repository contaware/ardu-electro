/*
  VL53L0X distance sensor module

  - VDD supply is 2.6V - 5.5V.
  
  - The violet VL53L0XV2 module has a 2.8V regulator and integrated level-shifters.
*/
#include "Adafruit_VL53L0X.h"
 
Adafruit_VL53L0X lox = Adafruit_VL53L0X();
const uint16_t maxDistance  = 1200; // for VL53L0X_SENSE_LONG_RANGE it is 2000, for all the others 1200
bool measuring = false;
unsigned long startMeasurementMicros;

void setup()
{
  // Serial Debug
  Serial.begin(115200);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!

  // Init VL53L0X
  Serial.println(F("VL53L0X Test - Non Blocking - Single"));
  /*
    Usually I2C address is 0x29
    VL53L0X_SENSE_DEFAULT
    VL53L0X_SENSE_LONG_RANGE
    VL53L0X_SENSE_HIGH_SPEED
    VL53L0X_SENSE_HIGH_ACCURACY
  */
  if (!lox.begin(VL53L0X_I2C_ADDR, false, &Wire, Adafruit_VL53L0X::VL53L0X_SENSE_DEFAULT))
  {
    Serial.println(F("Failed to boot VL53L0X"));
    while (true);
  }
  Serial.println();
}

void loop()
{
  unsigned long startMicros;
  unsigned long endMicros;
  
  if (!measuring)
  {
      // Start a non-blocking single range measurement
      startMicros = micros();
      measuring = lox.startRange();
      endMicros = micros();
  
      // startRange() time
      if (measuring)
      {
        startMeasurementMicros = endMicros;
        Serial.print(F("Start measurement (startRange() takes "));
      }
      else
        Serial.print(F("Failed to start measurement (startRange() takes "));
      Serial.print(endMicros - startMicros);
      Serial.println(F("us)"));
  }
  else
  {
    startMicros = micros();
    bool ok = lox.isRangeComplete(); // note: when this returns true, then it continues to return true until you call lox.readRangeResult()
    endMicros = micros();
    if (ok)
    {
      // Clear flag
      measuring = false;
      
      // Measurement + lox.isRangeComplete() time
      Serial.print(F("Measurement ready (measurement + isRangeComplete() take "));
      Serial.print(startMicros - startMeasurementMicros);
      Serial.print(F("us + "));
      Serial.print(endMicros - startMicros);
      Serial.println(F("us)"));
  
      // Read the result
      startMicros = micros();
      uint16_t result = lox.readRangeResult();
      endMicros = micros();
  
      // Check result (on error or on out of range it returns 0xffff)
      Serial.print(F("Distance: "));
      if (result <= maxDistance) // distances above maxDistance are unreliable
      {
        Serial.print(result);
        Serial.print(F("mm"));
      }
      else
        Serial.print(F("out of range"));
  
      // readRangeResult() time
      Serial.print(F(" (readRangeResult() takes "));
      Serial.print(endMicros - startMicros);
      Serial.println(F("us)"));

      Serial.println();
    }
    else
    {
      // isRangeComplete() time
      Serial.print(F("Measurement NOT ready (isRangeComplete() takes "));
      Serial.print(endMicros - startMicros);
      Serial.println(F("us)"));
    }
  }

  delay(1);
}
