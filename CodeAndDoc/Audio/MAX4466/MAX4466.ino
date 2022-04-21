/*
  MAX4466 microphone module with adjustable gain

  - VDD supply is 2.4V - 5.5V.
  - The output will have a DC bias of VDD / 2 (verifiable with the code below).
*/

const byte MIC_PIN = A0;

const int sampleWindow = 50; // sample window width in ms (50 ms = 20 Hz)

void setup() 
{
   Serial.begin(9600);
}

void loop() 
{
  unsigned long startMillis = millis();  // start of sample window
  int peakToPeak = 0;                    // peak-to-peak level
  int signalMax = 0;
  int signalMin = 1024;
  unsigned long signalAvg = 0;
  unsigned long count = 0;
  
  // Collect data for 50 ms
  while (millis() - startMillis < sampleWindow)
  {
    int sample = analogRead(MIC_PIN);
    
    // Avg
    signalAvg += sample;
    ++count;

    // Peak to peak
    if (sample > signalMax)
      signalMax = sample;
    else if (sample < signalMin)
      signalMin = sample;
  }

  // Calc
  if (count > 0)
    signalAvg /= count; 
  peakToPeak = signalMax - signalMin;
  
  // Print
  Serial.print("Avg: ");
  Serial.print(signalAvg * 5.0 / 1024);
  Serial.print(" , Peak to peak: ");
  Serial.print(peakToPeak * 5.0 / 1024);
  Serial.print(" , Samples: ");
  Serial.println(count); 
}
