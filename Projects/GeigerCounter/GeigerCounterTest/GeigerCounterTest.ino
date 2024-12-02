/*
  Geiger counter for beta and gamma radiations
  RadiationD-v1.1 (CAJOE) 

  **** ATTENTION: THERE ARE UP TO 500 VDC ON THE TUBE ****

  - VDD supply is 5V, used current is 12mA - 30mA. 
         
  - This detector can drive M4011, STS-5, SBM20 or J305 Geiger Müller
    tubes. Has voltage compatibility with all popular GM tubes that
    require an anode voltage between 350VDC - 500VDC.

  - Removing jumper J1 switches off the buzzer.
  
  - Jumper J4 is used for calibration.

  - There is a sliding switch which interrupts the 5V from the DC power
    connector and from the nearby terminal block. Note that the 5V pin
    close to the VIN/INT pin is always on.
    
  - There are 3 pins to connect to an Arduino: VIN/INT, GND and 5V.
    The kit sends nice and clean 400 uS high-low-high pulses with a 
    high impedance (470kΩ). We can connect it directly to a 3.3V 
    Arduino because it has such a high impedance, but if you want
    to be really safe, add a 1MΩ resistor from VIN/INT to ground
    to create a voltage divider.

  - The GM tube M4011 conversion index is 151, which means that
    151 CPM = 1μSv/h. The M4011 tube has an own background of around
    25 CPM.
    
  - 0.274 μSv/h = 2.4 mSv/year        : Worldwide avg natural dose
    5.7-11.4 μSv/h = 50-100 mSv/year  : Max yearly dose before health damage 
    100 μSv                           : The radiation during a x-ray
*/
#include <SimplyAtomic.h> // https://github.com/wizard97/SimplyAtomic

const unsigned long ONE_MINUTE_MS = 60000;
const unsigned long SAMPLE_PERIOD_MS = 20000;
const byte DET_PIN = 2;
volatile unsigned long count = 0;
unsigned long previousMillis;

void detectedISR()
{
  count++;
}

void setup()
{
  // Init Serial
  Serial.begin(9600);

  // Counter
  pinMode(DET_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(DET_PIN), detectedISR, FALLING);
  previousMillis = millis();
}

void loop()
{
  unsigned long currentMillis = millis();
  unsigned long diff = currentMillis - previousMillis;
  if (diff >= SAMPLE_PERIOD_MS)
  {
    // Read and clear the count as fast a possible.
    // Note: the following disables interrupts; if an interrupt happens while the interrupts
    //       are disabled it is handled when the interrupts are re-enabled, but if two or 
    //       more interrupts happen while the interrupts are disabled only one will be handled.
    unsigned long nowCount;
    ATOMIC()
    {
      nowCount = count;
      count = 0;
    }

    // Print
    unsigned long nowCPM = nowCount * (ONE_MINUTE_MS / diff);
    Serial.print("CPM ");
    Serial.print(nowCPM);
    Serial.print(" , ");
    Serial.print((float)nowCPM / 151.0); // to μSv/h
    Serial.println(" μSv/h (worldwide avg 0.274 µSv/h)");
    
    // Update previousMillis
    previousMillis = currentMillis;
  }
}
