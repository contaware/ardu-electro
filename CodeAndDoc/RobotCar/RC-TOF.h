/*  
  VL53L0X Distance Sensor
*/
#ifndef rc_tof_h
#define rc_tof_h

#include "RC-Config.h"

#if USE_VL53L0X_SENSOR == true

#include "TimerPoll.h"

const float WALL_MIN_DISTANCE_CM = 3.0;   // the datasheet does not specify a minimum range, but the effective limit is about 3.0 cm
const float WALL_MAX_DISTANCE_CM = 120.0; // for VL53L0X_SENSE_LONG_RANGE it is 200.0 cm, for all the others 120.0 cm

extern float g_wallDistanceCm; // Distance in cm, -1 on invalid value or when not yet calculated
extern TimerPoll g_tofTimer;

extern void tofBegin();
extern void tofMeasure(unsigned long elapsedTimeMs);

#endif
#endif
