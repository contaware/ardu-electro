#include "RC-US.h"

#if USE_ULTRASONIC_SENSOR == true

float g_wallDistanceCm = -1; // Distance in cm, -1 on invalid value or when not yet measured
TimerPoll g_ultrasonicTimer;

void ultrasonicBegin()
{
  pinMode(ULTRASONIC_ECHO_PIN, INPUT);
  pinMode(ULTRASONIC_TRIG_PIN, OUTPUT);
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
  g_ultrasonicTimer.begin(40, ultrasonicMeasure);
}

void ultrasonicMeasure(unsigned long elapsedTimeMs)
{
  // Write a pulse to the HC-SR04 Trigger Pin
  digitalWrite(ULTRASONIC_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);

  // Measure the response from the HC-SR04 Echo Pin
  float duration = pulseIn(ULTRASONIC_ECHO_PIN, HIGH); // duration in microseconds
  
  // Determine distance from duration (use 343 m/s as speed of sound)
  float distance = (duration / 2) * 0.0343;
  
  // Update wall distance
  if (distance >= WALL_MAX_DISTANCE_CM || distance <= WALL_MIN_DISTANCE_CM)
    g_wallDistanceCm = -1.0;
  else
  {
    g_wallDistanceCm = distance;
    DPRINT(F("HC-SR04 g_wallDistanceCm="));
    DPRINT(g_wallDistanceCm);
    DPRINTLN(F("cm"));
  }
}

#endif
