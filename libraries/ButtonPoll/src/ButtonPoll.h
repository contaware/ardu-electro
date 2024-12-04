/*
  Debounce a switch connected to ground
*/
#ifndef buttonpoll_h
#define buttonpoll_h

#include "Arduino.h"

class ButtonPoll
{
  private:
    byte m_pin = 0;
    uint16_t m_state = 0;
    unsigned long m_lastDebounceCallMs = 0;
    const unsigned long DEBOUNCE_CALL_DELAY_MS = 6; // 6 x 8 = 48ms debounce time
    
  public:
    ButtonPoll()
    {
      // Never call pinMode in constructor as hardware is not yet initialized
      // (it may work for AVR but for other platforms not)
    }
    
    void begin(byte buttonPin)
    {
      m_pin = buttonPin;
      m_state = 0;
      m_lastDebounceCallMs = millis();
      pinMode(buttonPin, INPUT_PULLUP);
    }
    
    bool pressed()
    {
      /* 
        The below digitalRead() executes in around 4-5μs, we have to introduce 
        a delay here otherwise the below 8 reads are not enough debounce time.  
      */
      unsigned long currentDebounceCallMs = millis();
      if ((currentDebounceCallMs - m_lastDebounceCallMs) < DEBOUNCE_CALL_DELAY_MS)
        return false;
      else
        m_lastDebounceCallMs = currentDebounceCallMs;
      
      /*
        When the user pushes the button the stream changes to a bouncy 
        pattern of ones and zeroes, but at some point there's the last 
        bounce (a one) followed by a stream of zeroes. We OR in 0xfe00 
        to create a "don't care" condition in the upper 7 bits, and thus 
        only the lower 9 bits get verified.
      */
      uint16_t value = digitalRead(m_pin) == HIGH ? 1 : 0;
      m_state = (m_state << 1) | value | 0xfe00;
      return (m_state == 0xff00);
    }
};

#endif
