/*
  The HC-020K module returns LOW when the beam passes (not interrupted)
  and HIGH when it is interrupted.
  My functional tests to discover the correct distance between the 
  wheel and the U-shaped sensor resulted in: 
 
  U-shaped sensor
      ________
     |        | this distance 
     |   ¦¦   |
         ¦¦
   ------¦¦
         ¦¦
         ¦¦ wheel
        
   Min:  5 mm
   Avg:  6.25 mm
   Max:  7.5 mm
   
   -> use a 6 mm screw to set the distance

  Note: there are many modules with different sensor heights, so always place them 
        in between of their Min and Max distances.
*/
#include <util/atomic.h>

const byte ENCODER_PIN = 2;
const unsigned long CHANGES_PER_TURN = 2 * 20; // 2 * number of slots on the disc
const unsigned long CALCULATION_TIME_MS = 1000;
volatile int g_changes;
volatile unsigned long g_lastChangesIncMs;
unsigned long g_lastCalculationMs;

// To debounce we could also use micros() because a debounce of 100 us is far enough 
// according to my oscilloscope tests. But micros() is slower than millis() which 
// just reads the milliseconds counter. The maximum RPM of the motor is 200 rpm,
// which with our slotted disc gives a frequency of 200 / 60 * 20 = 66 Hz. So the
// period of our square wave is 15 ms or 7.5 ms per logical state. The debounce
// time of 2-3 ms is ok because it is less than 7.5 ms.
const unsigned long DEBOUNCE_TIME_MS = 2;

void countChanges()
{
  unsigned long currentMs = millis();
  if ((currentMs - g_lastChangesIncMs) >= DEBOUNCE_TIME_MS)
  {
    g_lastChangesIncMs = currentMs;
    g_changes++;
  }
}

void setup()
{
  Serial.begin(9600);
  pinMode(ENCODER_PIN, INPUT);

  g_changes = 0;
  g_lastCalculationMs = millis();
  g_lastChangesIncMs = g_lastCalculationMs - DEBOUNCE_TIME_MS; // this correctly wraps back and assures that the ISR is called with the first change
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN), countChanges, CHANGE); // use CHANGE and not FALLING because the bounces on both edges are triggering FALLING
}

void loop()
{
  unsigned long currentMs = millis();
  unsigned long diffMs = currentMs - g_lastCalculationMs;
  if (diffMs >= CALCULATION_TIME_MS)
  {
    // Store last calculation time
    g_lastCalculationMs = currentMs;

    // Get changes count and reset it for next calculation
    int currentChanges;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
      currentChanges = g_changes;
      g_changes = 0;
    }
    
    // RPM
    unsigned long rpm = (60 * 1000UL / CHANGES_PER_TURN) * (unsigned long)currentChanges / diffMs;
    
    // Print
    Serial.print("Changes: ");
    Serial.println(currentChanges);
    Serial.print("RPM: ");
    Serial.println(rpm);
    Serial.println();
  }
}
