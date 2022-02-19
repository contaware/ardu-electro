#include "TimerPoll.h"

TimerPoll timerSingleShot;
TimerPoll timerTwoShots;
TimerPoll timerPeriodic;

const unsigned long DELAY_TIME_MS = 2000; // 2 sec

void setup()
{
  Serial.begin(9600);
  timerSingleShot.begin(DELAY_TIME_MS / 6, singleShot, 1);
  timerTwoShots.begin(DELAY_TIME_MS / 3, twoShots, 2);
  timerPeriodic.begin(DELAY_TIME_MS, periodic);
}

void singleShot(unsigned long elapsedTimeMs)
{
  Serial.print("singleShot() triggerd after ");
  Serial.print(elapsedTimeMs);
  Serial.println(" ms");
}

void twoShots(unsigned long elapsedTimeMs)
{
  Serial.print("twoShots() triggerd after ");
  Serial.print(elapsedTimeMs);
  Serial.println(" ms");
}

void periodic(unsigned long elapsedTimeMs)
{
  Serial.print("periodic() triggerd after ");
  Serial.print(elapsedTimeMs);
  Serial.println(" ms");
}

void loop()
{
  timerSingleShot.process();
  timerTwoShots.process();
  timerPeriodic.process();
}
