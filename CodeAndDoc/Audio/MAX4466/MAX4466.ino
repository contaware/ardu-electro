/*
  MAX4466 mic preamp with adjustable gain, view the audio on the Serial Plotter

  - VDD supply is 2.4V - 5.5V.
  
  - The output will have a DC bias of VDD / 2.
*/

const byte MIC_PIN = A0;
float avg = 511.0;
unsigned long prevMillis;
int sampleMax;    
int sampleMin;

void setup() 
{
  // A baud rate of 115200 is used instead of 9600 for a faster data rate
  Serial.begin(115200);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!

  // Reset
  prevMillis = millis();
  sampleMax = 0;
  sampleMin = 1024;
}

void loop()
{
  // Read sample
  int sample = analogRead(MIC_PIN);

  // Max and Min
  if (sample > sampleMax)
    sampleMax = sample;
  else if (sample < sampleMin)
    sampleMin = sample;

  // Avg
  // Calc. exponentially weighted moving average (EWMA) with n + 1 = 4096
  // avg = (sample + n * avg) / (n + 1) = avg + (sample - avg) / (n + 1)
  avg += (sample - avg) / 4096.0;

  // Avoid overflowing the serial port by printing each 10 ms
  unsigned long currentMillis = millis();
  if (currentMillis - prevMillis >= 10)
  {
    // Print
    Serial.print("h:1024,s:");
    Serial.print(sampleMax - sampleMin);
    Serial.print(",a:");
    Serial.print(avg, 1);
    Serial.println(",l:0");

    // Reset
    prevMillis = currentMillis;
    sampleMax = 0;
    sampleMin = 1024;
  } 
}
