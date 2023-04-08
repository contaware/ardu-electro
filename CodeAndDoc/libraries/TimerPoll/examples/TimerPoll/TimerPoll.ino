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

void singleShot()
{
  Serial.println("singleShot() triggered");
}

void twoShots()
{
  Serial.println("twoShots() triggered");
}

void periodic()
{
  Serial.println("periodic() triggered");
}

void loop()
{
  timerSingleShot.process();
  timerTwoShots.process();
  timerPeriodic.process();
}
