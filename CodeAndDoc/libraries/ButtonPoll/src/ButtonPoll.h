/*
  Debounce a switch connected to ground
 
  Enhanced the following library with m_lastDebounceCallMs: 
  https://github.com/e-tinkers/button
  https://www.e-tinkers.com/2021/05/the-simplest-button-debounce-solution/
  http://www.ganssle.com/debouncing-pt2.htm
  https://www.eejournal.com/article/ultimate-guide-to-switch-debounce-part-4/
*/
#ifndef buttonpoll_h
#define buttonpoll_h

#include "Arduino.h"

class ButtonPoll
{
  private:
    byte m_pin;
    uint16_t m_state;
    unsigned long m_lastDebounceCallMs;
    const unsigned long DEBOUNCE_CALL_DELAY_MS = 6; // 6 x 8 = 48 ms debounce time
    
  public:
    ButtonPoll()
    {
      m_pin = 0;
      m_state = 0;
      m_lastDebounceCallMs = 0;
      // Never call pinMode in constructor as hardware is not yet initialized
      // (it may work for AVR but for other platforms not)
    }
    
    void begin(byte buttonPin)
    {
      m_pin = buttonPin;
      m_state = 0;
      m_lastDebounceCallMs = 0;
      pinMode(buttonPin, INPUT_PULLUP);
    }
    
    bool pressed()
    {
      /* 
        The below digitalRead() executes in around 4-5us, we have to introduce 
        a delay here otherwise the below 8 reads are not enough debounce time.  
      */
      unsigned long currentDebounceCallMs = millis();
      if ((currentDebounceCallMs - m_lastDebounceCallMs) < DEBOUNCE_CALL_DELAY_MS)
        return false;
      else
        m_lastDebounceCallMs = currentDebounceCallMs;
      
      /*
        When the user pushes on the button the stream changes to a bouncy pattern of ones and zeroes, 
        but at some point there's the last bounce (a one) followed by a stream of zeroes. We OR in 
        0xfe00 to create a "don't care" condition in the upper bits. There's just the one time,
        when the last bouncy "one" was in the upper bit position, that the code returns a true. 
        That bit of wizardry eliminates bounces and detects the edge from open to closed.
      */
      m_state = (m_state << 1) | (uint16_t)digitalRead(m_pin) | 0xfe00;
      return (m_state == 0xff00);
    }
};

#endif
