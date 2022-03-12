/*
  VL53L0X Distance Sensor
*/
#include "Adafruit_VL53L0X.h"
 
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

const uint16_t maxDistance  = 1200; // for VL53L0X_SENSE_LONG_RANGE it is 2000, for all the others 1200
 
void setup()
{
  Serial.begin(9600);
  
  Serial.println(F("VL53L0X Test - Non Blocking"));
  /*
    VL53L0X_SENSE_DEFAULT
    VL53L0X_SENSE_LONG_RANGE
    VL53L0X_SENSE_HIGH_SPEED
    VL53L0X_SENSE_HIGH_ACCURACY
  */
  if (!lox.begin(VL53L0X_I2C_ADDR, false, &Wire, Adafruit_VL53L0X::VL53L0X_SENSE_DEFAULT))
  {
    Serial.println(F("Failed to boot VL53L0X"));
    while(true);
  }
  Serial.println();

  // Range continuously each 1 sec
  lox.startRangeContinuous(1000); // default period is 50 ms
}

void loop()
{
  unsigned long startMs = micros();
  boolean ok = lox.isRangeComplete(); // note: when this returns true, then it continues to return true until you call lox.readRangeResult()
  unsigned long endMs = micros();
  if (ok)
  { 
    // Performance
    Serial.print(F("Measurement ready (isRangeComplete() takes "));
    Serial.print(endMs - startMs);
    Serial.println(F("us)"));

    // Read the result
    startMs = micros();
    uint16_t result = lox.readRangeResult();
    endMs = micros();

    // Check result (on error or on out of range it returns 0xffff)
    if (result <= maxDistance) // distances above maxDistance are unreliable
    {
      Serial.print(F("Distance: "));
      Serial.print(result);
      Serial.print(F("mm"));
    }
    else
      Serial.print(F("*** out of range ***"));

    // Performance
    Serial.print(F(" (readRangeResult() takes "));
    Serial.print(endMs - startMs);
    Serial.println(F("us)"));
  }
  else
  {
    // Performance
    Serial.print(F("Measurement NOT ready (isRangeComplete() takes "));
    Serial.print(endMs - startMs);
    Serial.println(F("us)"));
  }
  
  Serial.println();
  
  delay(200);
}
