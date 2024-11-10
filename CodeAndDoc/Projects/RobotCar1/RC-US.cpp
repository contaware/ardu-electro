#include "RC-US.h"

#if USE_ULTRASONIC_SENSOR == true

#include "RC-Motor.h"

TimerPoll g_ultrasonicTimer;

void ultrasonicBegin()
{
  pinMode(ULTRASONIC_ECHO_PIN, INPUT);
  pinMode(ULTRASONIC_TRIG_PIN, OUTPUT);
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
  g_ultrasonicTimer.begin(40, ultrasonicMeasure);
}

void ultrasonicMeasure()
{
  // Write a pulse to the HC-SR04 Trigger Pin
  digitalWrite(ULTRASONIC_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);

  // Measure the response from the HC-SR04 Echo Pin
  float duration = pulseIn(ULTRASONIC_ECHO_PIN, HIGH); // duration in microseconds
  
  // Determine distance from duration (use 343 m/s as speed of sound)
  float distanceCm = (duration / 2) * 0.0343;

  // Update force brake state
  if (!g_forceBrake && distanceCm < ULTRASONIC_WALL_BRAKE_CM)
  {
    g_forceBrake = true;

    // Do brake if going forward
    if (g_motorLeftState > 0 && g_motorRightState > 0)
    {
      motorLeftState(0);
      motorRightState(0);
      motorSpeed(255);
    }

    // Debug print
    DPRINT(F("g_forceBrake=true @ "));
    DPRINT(distanceCm);
    DPRINTLN(F("cm"));
  }
  else if (g_forceBrake && distanceCm > ULTRASONIC_WALL_RELEASE_CM)
  {
    g_forceBrake = false;

    // Debug print
    DPRINT(F("g_forceBrake=false @ "));
    DPRINT(distanceCm);
    DPRINTLN(F("cm"));
  }
}

#endif
