/*
  Typically global variables are used to pass data between an ISR and the main program.
  To make sure variables shared between an ISR and the main program are updated correctly,
  declare them as volatile.
  When accesing volatile variables from loop() which are greater than the CPU bits 
  (8-bits for UNO and MEGA) then we have to disable the interrupts with noInterrupts() and 
  re-enable them with interrupts(). That works well because interrupt events can occur at 
  any time and are remembered by setting an "interrupt event" flag inside the processor. 
  If interrupts are disabled, they will be handled when they are enabled again.
 
  In complex code it's always better to verify whether before disabling the interrupts
  they were enabled or not:
 
  byte oldSREG = SREG; // remember if interrupts are on or off
  noInterrupts();      // turn interrupts off
  // do atomic stuff here
  SREG = oldSREG;      // turn interrupts back on, if they were on before
 
  or:
 
  #include <util/atomic.h>
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    // do atomic stuff here
  }
 
  Good reference:
  http://gammon.com.au/interrupts 
*/
#include <util/atomic.h>

/*
  Digital Pins Usable For Interrupts
  UNO:  2, 3
  MEGA: 2, 3, 18, 19, 20, 21 (pins 20 & 21 are not available to use for interrupts while they are used for I2C communication)
*/
const byte INTERRUPT_PIN = 2;

volatile unsigned long count = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);

  /*
    LOW to trigger the interrupt whenever the pin is low
    CHANGE to trigger the interrupt whenever the pin changes value
    RISING to trigger when the pin goes from low to high
    FALLING for when the pin goes from high to low
  */
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), my_isr, CHANGE);
}

void loop()
{
  unsigned long current_count;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    current_count = count;
  }
  Serial.print("Interrupt count = ");
  Serial.println(current_count);
  delay(1000); // note: interrupts still work while waiting inside the delay function
}

void my_isr()
{
  /*
    - Generally, an ISR should be as short and fast as possible.
    - If your sketch uses multiple ISRs, only one can run at a time, other interrupts 
      will be executed after the current one finishes in an order that depends on the 
      priority they have; different ISR will not interrupt each other because before 
      entering an ISR all interrupts are disabled.
      https://stackoverflow.com/questions/5111393/do-interrupts-interrupt-other-interrupts-on-arduino
    - Don't do serial prints.
    - millis() relies on interrupts to count, so it will never increment inside an ISR.
      micros() works initially but will start behaving erratically after 1-2 ms.
      https://arduino.stackexchange.com/questions/22212/using-millis-and-micros-inside-an-interrupt-routine
    - delay() requires interrupts to work, it will not work if called inside an ISR.
      delayMicroseconds() will work as normal because it loops a calculated amount 
      (knowing CPU frequency) of wait instructions.
      https://forum.arduino.cc/t/millis-and-micros-and-delaymicroseconds-inside-isr/261838/9
  */
  count++;
}
