/*
  Rotary encoder with KY-040 module

  For this code we got inspired by:
  https://www.best-microcontroller-projects.com/rotary-encoder.html
  An alternative is to use a state machine:
  http://www.buxtronix.net/2011/10/rotary-encoders-done-properly.html
 
                  CW               CCW
  Position     CLK   DT         CLK   DT
  0            1     1          1     1
  1/4          1     0          0     1
  1/2          0     0          0     0
  3/4          0     1          1     0
  1            1     1          1     1
  
  Prev State Next State    Valid code    Direction
  CLK DT     CLK DT
  0000                     =             X
  0001                     Valid         CW
  0010                     Valid         CCW
  0011                     X             X
  0100                     Valid         CCW
  0101                     =             X
  0110                     X             X
  0111                     Valid         CW
  1000                     Valid         CW
  1001                     X             X
  1010                     =             X
  1011                     Valid         CCW
  1100                     X             X
  1101                     Valid         CCW
  1110                     Valid         CW
  1111                     =             X
*/
#ifndef rotaryencoderpoll_h
#define rotaryencoderpoll_h

#include "Arduino.h"

class RotaryEncoderPoll
{
  private:
    byte m_pinCLK;
    byte m_pinDT;
    byte m_firstSecondThirdCode = 0b00111111;
    const byte m_encSeqTable[16] = {2, 1, 1, 0, 1, 2, 0, 1, 1, 0, 2, 1, 0, 1, 1, 2}; // 0: invalid, 1: valid, 2: same
    
  public:
    RotaryEncoderPoll()
    {
      m_pinCLK = 0;
      m_pinDT = 0;
      m_firstSecondThirdCode = 0b00111111;
      // Never call pinMode in constructor as hardware is not yet initialized
      // (it may work for AVR but for other platforms not)
    }
    
    void begin(byte pinCLK, byte pinDT)
    {
      m_pinCLK = pinCLK;
      m_pinDT = pinDT;
      m_firstSecondThirdCode = 0b00111111;
      pinMode(pinCLK, INPUT);
      pinMode(pinDT, INPUT);
    }
    
    /*
	  To return a valid rotation (1 or -1) we always consider the current
      state in relation to the last two ones. This results in debounce and 
      also validates the rotation direction.
    */
    int read()
    {
      // Make room for new two bits
      m_firstSecondThirdCode <<= 2;
    
      // Read the new values placing them in third position
      if (digitalRead(m_pinDT))
        m_firstSecondThirdCode |= 0b01;
      if (digitalRead(m_pinCLK))
        m_firstSecondThirdCode |= 0b10;
        
      // Test second and third codes
      byte seq = m_encSeqTable[m_firstSecondThirdCode & 0x0f];
      if (seq == 0)                           // invalid
        m_firstSecondThirdCode |= 0b00111100; // -> drop first and second, only keep third
      else if (seq == 2)                      // same
        m_firstSecondThirdCode >>= 2;         // -> remove the new duplicated code
      else
      {
        // Test all three codes
        if ((m_firstSecondThirdCode & 0b00111111) == 0b00000111)
          return 1; // CW
        else if ((m_firstSecondThirdCode & 0b00111111) == 0b00001011)
          return -1; // CCW
        //else not yet at the end of the new rotation position
      }
      
      return 0;
    }
};

#endif
