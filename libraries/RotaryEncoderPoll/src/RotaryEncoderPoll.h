/*
  Rotary encoder with KY-040 module
  
  - This library validates and debounces the rotary encoder using a 
    state machine. The push button of the rotary encoder must be 
    debounced with the separate ButtonPoll library (see example code).

  - The CLK & DT outputs (some modules have CW and CCW inverted):
  
                    CW                 CCW
    Position     CLK   DT           CLK   DT
    0            1     1 <-         1     1 <-
    1/4          1     0   |        0     1   |
    1/2          0     0   |        0     0   |
    3/4          0     1 __|        1     0 __|
  
  - Placing two consecutive states in a 4-bit variable makes the 
    16 indices for the m_encSeqTable:
  
    Prev    Next      Valid       Direction
    State   State     code?
    CLK DT  CLK DT
    |   /   /   /
    | _/   /   /
    ||    /   /
    || __/   /
    ||| ____/
    ||||
    vvvv
    0000              =           X
    0001              Valid       CW
    0010              Valid       CCW
    0011              X           X
    0100              Valid       CCW
    0101              =           X
    0110              X           X
    0111              Valid       CW
    1000              Valid       CW
    1001              X           X
    1010              =           X
    1011              Valid       CCW
    1100              X           X
    1101              Valid       CCW
    1110              Valid       CW
    1111              =           X
*/
#ifndef rotaryencoderpoll_h
#define rotaryencoderpoll_h

#include "Arduino.h"

class RotaryEncoderPoll
{
  private:
    // Three 2-bit codes are stored in a byte, and the two most 
    // significant bits are used in case we restore by shifting right 
    // after a left shift.
    // Code positions:                1°2°3°
    byte m_firstSecondThirdCode = 0b00111111;
    
    // Pins
    byte m_pinCLK = 0;
    byte m_pinDT = 0;
    
    // Table values: 0=invalid, 1=valid, 2=same
    const byte m_encSeqTable[16] = {2, 1, 1, 0, 1, 2, 0, 1, 1, 0, 2, 1, 0, 1, 1, 2};
    
  public:
    RotaryEncoderPoll()
    {
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
      state in relation to the last two. This gives debouncing and also 
      validates the rotation direction.
    */
    int read()
    {
      // Make room for the new two bits by shifting left
      // Note: the old first code is not yet lost, as it now occupies 
      //       the two most significant positions.
      m_firstSecondThirdCode <<= 2;
    
      // Read the new values placing them in third position
      if (digitalRead(m_pinDT) == HIGH)
        m_firstSecondThirdCode |= 0b01;
      if (digitalRead(m_pinCLK) == HIGH)
        m_firstSecondThirdCode |= 0b10;
        
      // Test the third code by comparing it with the second one
      byte seq = m_encSeqTable[m_firstSecondThirdCode & 0x0f];
      if (seq == 0)                           // invalid
        m_firstSecondThirdCode |= 0b00111100; // -> drop first and second, only keep third
      else if (seq == 2)                      // same
        m_firstSecondThirdCode >>= 2;         // -> restore (removes duplicated code)
      else
      {
        // Test the three codes checking only the two combinations that 
        // end with the 11 code because that's the stable end position
        if ((m_firstSecondThirdCode & 0b00111111) == 0b00000111)
          return 1; // CW
        else if ((m_firstSecondThirdCode & 0b00111111) == 0b00001011)
          return -1; // CCW
        //else not yet at the end of the new rotation
      }
      
      return 0;
    }
};

#endif
