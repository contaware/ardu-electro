/*
  ATtiny85 showcase using ATTinyCore (1.5.2)

                     ----u----
       RESET/A0/PB5 |1       8| VCC
          SS/A3/PB3 |2       7| PB2/A1/SCL/SCK/INT0
             A2/PB4 |3~     ~6| PB1/LED_BUILTIN/RX/MISO
                GND |4      ~5| PB0/AREF/TX/SDA/MOSI
                     ---------
              ATtiny85 (~ = PWM pins)

  - Digital I/O
    0 (PB0)
    1 (PB1)
    2 (PB2)
    3 (PB3)
    4 (PB4)
    5 (PB5)
    Interrupts: digitalPinToInterrupt() returns 0 (INT0) only for 
    digital pin 2, that's the only fully featured external interrupt 
    pin. Pin change interrupts (PCINTn) only trigger on change (not 
    edge or level) and have limited functionality, they cannot be 
    used with attachInterrupt().
  
  - PWM
    0 (PB0)  488Hz @ 1MHz and 8MHz clock, 977Hz @ 16MHz clock
    1 (PB1)  488Hz @ 1MHz and 8MHz clock, 977Hz @ 16MHz clock
    4 (PB4)  488Hz @ 1MHz and 8MHz clock, 977Hz @ 16MHz clock
  
  - ADC
    A0 (PB5)
    A1 (PB2)
    A2 (PB4)
    A3 (PB3)

  - SPI
    The above chip marking are for when the SPI is in slave mode 
    (ISP programming), in master mode MISO and MOSI are swapped:
    MISO (PB0)
    MOSI (PB1)
    SCK  (PB2)
    SS   (PB3)

  - I2C
    SDA (PB0)
    SCL (PB2)

  - Serial
    Attention: it's a software implementation, you cannot send or 
    receive at the same time.
    TX (PB0)
    RX (PB1)
*/
#include <util/atomic.h>        // this library includes the ATOMIC_BLOCK macro

// Pins
#define INTERRUPT_PIN           2
#define PWM_PIN                 4
#define ANALOG_PIN              A3

// Last millis
unsigned long lastMillis;

// Use volatile for global variables shared between setup()/loop() and ISR
volatile uint32_t count1 = 0;
volatile uint32_t count2 = 0;

void myISR()
{
  count1++;
  count2++;
}

void setup()
{
  // Init Serial (leave Serial Monitor open to see all messages)
  Serial.begin(9600); delay(5000); // wait 5s that Serial is ready

  // Print F_CPU value
  Serial.print(F("CPU: "));
  Serial.print(F_CPU / 1000000L);
  Serial.println(F("MHz"));

  // ADC reference
  // DEFAULT:     VCC
  // EXTERNAL: 	  Voltage applied to AREF pin
  // INTERNAL1V1: Internal 1.1V reference (alias: INTERNAL)
  analogReference(DEFAULT);

  // Set interrupt pin
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), myISR, FALLING);

  // Init millis
  lastMillis = millis();
}

void loop()
{
  // Analog read
  int analogValue = analogRead(ANALOG_PIN);

  // Analog write
  int pwmValue = map(analogValue, 0, 1023, 0, 255);
  analogWrite(PWM_PIN, pwmValue);

  // Print
  unsigned long currentMillis = millis();
  if (currentMillis - lastMillis > 4000)
  {
    lastMillis = currentMillis;

    // Print current analog value
    Serial.print(F("ADC: ")); Serial.println(analogValue);

    // Print interrupts count
    uint32_t currentCount1, currentCount2;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
      currentCount1 = count1;
      currentCount2 = count2;
      count2 = 0;
    }
    Serial.print(F("INT: total=")); Serial.print(currentCount1);
    Serial.print(F(" , last 4s=")); Serial.println(currentCount2);
  }
}
