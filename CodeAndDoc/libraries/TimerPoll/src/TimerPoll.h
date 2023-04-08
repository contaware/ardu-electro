/*
  Flexible Timer which can be used as a periodic timer 
  or as a single/multiple shots timer (no interrupts are used).
  The process() function must be polled in your loop() and will 
  trigger a callback function when the given time elapses.

  My personal opinion is to avoid using the internal timers 
  because 1. interrupts programming is more complicated and involves
  knowing about volatile variables and ATOMIC_BLOCK() and 2. they 
  are often already used by Arduino itself, for example the Arduino 
  Uno is using them all:

  - Timer0 is a 8bit timer used for the timer functions delay(), 
    millis() and micros()
 
  - Timer1 is a 16 bit timer used by the Servo library
    (Timer5 on Arduino Mega)
 
  - Timer2 is a 8bit timer used by the tone() function
*/
#ifndef timerpoll_h
#define timerpoll_h

#include <Arduino.h>
#include <limits.h>

class TimerPoll
{
  private:
    unsigned long m_delayTimeMs;
    void (*m_timerCallback)(void);
    unsigned long m_delayStartMs;
    unsigned long m_shotsCount;
    
  public:
    TimerPoll()
    {
      m_delayTimeMs = 0;
      m_timerCallback = nullptr;
      m_delayStartMs = 0;
      m_shotsCount = 0;
    }
    
    void begin(unsigned long delayTimeMs,             // timer delay in milliseconds
              void (*timerCallback)(void),   		  // callback
              unsigned long shotsCount = ULONG_MAX)   // if shotsCount is ULONG_MAX we have a periodic infinite timer
    {
      m_delayTimeMs = delayTimeMs;
      m_timerCallback = timerCallback;
      m_delayStartMs = millis();
      m_shotsCount = shotsCount;
    }
    
    void process()
    {
      if ((m_shotsCount > 0) && (millis() - m_delayStartMs >= m_delayTimeMs))
      {
        if (m_shotsCount < ULONG_MAX)
          m_shotsCount--;
        m_delayStartMs += m_delayTimeMs; // this prevents drift in the delays and correctly wraps around when reaching ULONG_MAX
        m_timerCallback();
      }
    }
};

#endif
