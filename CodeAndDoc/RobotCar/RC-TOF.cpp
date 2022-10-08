#include "RC-TOF.h"

#if USE_VL53L0X_SENSOR == true

#include "Adafruit_VL53L0X.h"

float g_wallDistanceCm = -1; // Distance in cm, -1 on invalid value or when not yet measured
TimerPoll g_tofTimer;
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

void tofBegin()
{
  // In Adafruit_VL53L0X::VL53L0X_SENSE_DEFAULT mode the sensor needs
  // around 33 ms to make a measurement
  if (!lox.begin(VL53L0X_I2C_ADDR, false, &Wire, Adafruit_VL53L0X::VL53L0X_SENSE_DEFAULT))
    DPRINTLN(F("Failed to boot VL53L0X"));

  // Range continuously, each 50 ms
  lox.startRangeContinuous(50);

  // Read it a bit slower, each 60 ms
  g_tofTimer.begin(60, tofMeasure);
}

void tofMeasure(unsigned long elapsedTimeMs)
{
  if (lox.isRangeComplete()) // note: when this returns true, then it continues to return true until you call lox.readRangeResult()
  {
    // Read the result (it returns mm)
    uint16_t result = lox.readRangeResult();

    // Check result (on error or on out of range it returns 0xffff)
    if (result >= (uint16_t)(WALL_MAX_DISTANCE_CM * 10.0) || result <= (uint16_t)(WALL_MIN_DISTANCE_CM * 10.0))
      g_wallDistanceCm = -1.0; // out of range
    else
    {
      g_wallDistanceCm = (float)result / 10.0;
      DPRINT(F("VL53L0X g_wallDistanceCm="));
      DPRINT(g_wallDistanceCm);
      DPRINTLN(F("cm"));
    }
  }
}

#endif
