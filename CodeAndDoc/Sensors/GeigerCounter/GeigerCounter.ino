/*
  Geiger counter for beta and gamma radiations
  RadiationD-v1.1 (CAJOE) 

  **** ATTENTION: THERE ARE UP TO 500 VDC ON THE TUBE ****

  - VDD supply is 5V. Used current is 12mA - 30mA. 
         
  - This detector can drive M4011, STS-5, SBM20 or J305 Geiger Müller
    tubes. Has voltage compatibility with all popular GM tubes that
    require an anode voltage between 350VDC - 500VDC.
 
  - There are 3 pins to connect to an Arduino: VIN/INT, GND and 5V.
    The kit sends nice and clean 500 uS high-low-high pulses, the 
    high voltage is 3.3V, which is enough to trigger a 5V Arduino.

  - The GM tube M4011 conversion index is 151, which means that
    151 CPM = 1μSv/h. The M4011 tube has an own background of around
    25 CPM.
    
  - 0.274 μSv/h = 2.4 mSv/year        : Worldwide avg natural dose
    5.7-11.4 μSv/h = 50-100 mSv/year  : Max yearly dose before health damage 
    100 μSv                           : The radiation during a x-ray
  
  - References:
    https://www.youtube.com/watch?v=K28Az3-gV7E
    https://github.com/SensorsIot/Geiger-Counter-RadiationD-v1.1-CAJOE-
*/
#include <util/atomic.h>

const unsigned long ONE_MINUTE_MS = 60000;    // ms
const unsigned long SAMPLE_PERIOD_MS = 60000; // ms
const byte DET_PIN = 2;

volatile unsigned long count = 0;
unsigned long previousMillis;

void detectedISR()
{
  count++;
}

void setup()
{
  pinMode(DET_PIN, INPUT);   
  Serial.begin(9600);
  
  previousMillis = millis();
  attachInterrupt(digitalPinToInterrupt(DET_PIN), detectedISR, FALLING);

  Serial.println("Started counting, please wait...");
}

void loop()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > SAMPLE_PERIOD_MS)
  {
    // Read and clear the count as fast a possible.
    // Note: the following disables interrupts; if an interrupt happens while the interrupts
    //       are disabled it is handled when the interrupts are re-enabled. The following 
    //       code is executed fast and more than one interrupt cannot happen while the 
    //       interrupts are disabled.
    unsigned long current_count;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
      current_count = count;
      count = 0;
    }

    // Print
    unsigned long CPM = current_count * (ONE_MINUTE_MS / SAMPLE_PERIOD_MS);
    float microSievertPerHour = (float)CPM / 151.0;
    Serial.print("CPM ");
    Serial.print(CPM);
    Serial.print(" , ");
    Serial.print(microSievertPerHour);
    Serial.println(" μSv/h (worldwide avg 0.274 µSv/h)");
    
    // Update previousMillis
    previousMillis = currentMillis;
  }
}
