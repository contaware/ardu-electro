/*
  Interrupts and atomic read/write
  
  - Typically global variables are used to pass data between an 
    ISR (Interrupt Service Routine) and the main program. To make sure 
    that variables shared between an ISR and the main program are 
    updated correctly, we must declare them as volatile.
  
  - Reading or writing volatile variables which are greater than the 
    MCU bitness (8-bits for AVRs) requires more than one machine 
    instruction. There is a chance that an ISR updates a volatile 
    variable exactly when the main program is reading it. 
    The solution is that before using a volatile variable, the main 
    program must turn off interrupts with noInterrupts() and then turn 
    them back on with interrupts().

  - Note that while the interrupts are disabled, interrupt events can 
    still occur and are remembered by the MCU which sets an interrupt 
    event flag. However, the time during which interrupts are disabled 
    must be as short as possible, because only one interrupt event per 
    interrupt number is retained.
 
  - In complex code, it's always better to verify whether before 
    disabling the interrupts they were enabled or not, for AVR:

    byte oldSREG = SREG; // remember if interrupts are on or off
    noInterrupts();      // turn interrupts off
    // ... do atomic stuff here
    SREG = oldSREG;      // turn interrupts back on, if they were on
 
  - The same as above, but with a handy macro:
 
    #include <util/atomic.h>
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
      // ... do atomic stuff here
    }
*/

// Unfortunately <util/atomic.h> is only available for AVR, thus we
// cannot use the ATOMIC_BLOCK(ATOMIC_RESTORESTATE) macro for 32-bit
// platforms. But there is a nice portable library which defines 
// an ATOMIC() macro that works for AVR and many 32-bit MCUs.
#include <SimplyAtomic.h> // https://github.com/wizard97/SimplyAtomic

// Choose a digital pin as the source of the interrupts
const byte INTERRUPT_PIN = 2; // pin 2 works for many platforms
                              // (pin 1 for MKR Family)

// Use volatile for global variables shared between setup()/loop() and ISR
volatile uint32_t count1 = 0;
volatile uint32_t count2 = 0;

void setup()
{
  // Serial init
  Serial.begin(9600);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!

  // Set pin mode
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);

  /*
    CHANGE:  to trigger an interrupt whenever the pin changes value.
    RISING:  to trigger an interrupt when the pin goes from low to high.
    FALLING: to trigger an interrupt when the pin goes from high to low.
    LOW:     triggers interrupts whenever the pin is low.
    HIGH:    triggers interrupts whenever the pin is high, 
             not supported by all boards.
    Attention: LOW and HIGH keep firing one interrupt after the other!
  */
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), myISR, FALLING);
}

void loop()
{
  // Interrupts still work while waiting inside the delay() function
  delay(4000);

  // Access a 32-bit variable
  // - For AVR, which is 8-bit, count1 must be accessed with the 
  //   ATOMIC() macro.
  // - For 32-bit MCUs that's not necessary because their 32-bit 
  //   accesses are already atomic.
  uint32_t currentCount1;
#if defined(__AVR__)
  ATOMIC()
#endif
  {
    currentCount1 = count1;
  }
  
  // Read + write a 32-bit variable (or access a 64-bit variable)
  // - For those cases the ATOMIC() macro is always needed.
  uint32_t currentCount2;
  ATOMIC()
  {
    currentCount2 = count2;
    count2 = 0;
  }

  // Serial print
  Serial.print("Total="); Serial.print(currentCount1);
  Serial.print(" , Last 4s="); Serial.println(currentCount2);
}

void myISR()
{
  /*
    - If your sketch uses multiple ISRs, only one can run at a time, 
      other interrupts will be executed after the current one finishes 
      in an order that depends on the priority they have; different ISR 
      will not interrupt each other because before entering an ISR all 
      interrupts are disabled:
      https://stackoverflow.com/questions/5111393/do-interrupts-interrupt-other-interrupts-on-arduino

    - An ISR should be as short and fast as possible because only one 
      additional interrupt event per interrupt number is retained by the 
      MCU's flags.
    
    - Don't do Serial prints in ISR, they are time consuming and on older 
      Arduino versions they deadlocked if the Serial buffer got full. 
      There has been some work to avoid deadlocks:
      https://github.com/arduino/Arduino/commit/ccd8880a37261b53ae11c666de0a29d85c28ae36
      but we do not know whether all platforms implement lock-free 
      Serial prints in ISRs.

    - * millis() relies on interrupts to count, so it will never 
        increment inside an ISR.
      * micros() works initially but will start behaving erratically 
        after 1-2 ms.
      https://arduino.stackexchange.com/questions/22212/using-millis-and-micros-inside-an-interrupt-routine
    
    - * delay() requires interrupts to work, it will not work if called 
        inside an ISR.
      * delayMicroseconds() will work as normal because it loops a 
        calculated amount (knowing CPU frequency) of wait instructions.
      https://forum.arduino.cc/t/millis-and-micros-and-delaymicroseconds-inside-isr/261838/10
  */
  count1++;
  count2++;
}
