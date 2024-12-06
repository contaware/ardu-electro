/*
  Set the frequency divisor for a given PWM pin
 
  - Pins 5 and 6 are paired on timer0 with base frequency 62500 Hz
    (default divisor is 64 -> 976 Hz)

  - Pins 9 and 10 are paired on timer1 with base frequency 31372.55 Hz
    (default divisor is 64 -> 490 Hz)

  - Pins 3 and 11 are paired on timer2 with base frequency 31372.55 Hz
    (default divisor is 64 -> 490 Hz)

  Note that this function will have side effects on anything else
  that uses timers:
  - timer0 is used for the time functions like delay(), millis(), micros()
  - timer1 is used by the Servo library
  - timer2 is used by the tone() function

  See: 
  https://www.arduino.cc/reference/en/language/functions/analog-io/analogwrite/
  https://arduinoinfo.mywikis.net/wiki/Arduino-PWM-Frequency
  https://docs.arduino.cc/tutorials/generic/secrets-of-arduino-pwm
*/
void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if(pin == 5 || pin == 6) {
      TCCR0B = (TCCR0B & 0b11111000) | mode;
    } else {
      TCCR1B = (TCCR1B & 0b11111000) | mode;
    }
  } else if(pin == 3 || pin == 11) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x07; break;
      default: return;
    }
    TCCR2B = (TCCR2B & 0b11111000) | mode;
  }
}
