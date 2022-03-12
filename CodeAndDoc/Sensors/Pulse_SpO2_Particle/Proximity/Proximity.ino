/*
  The chip runs at 1.8V, for the I2C interface some modules have level 
  shifters built-in, others have pull-ups to 1.8V or 3.3V.
  
  For the modules with 1.8V pull-ups some people suggest to disconnect
  the module pull-ups and do external pull-ups to 3.3V or 5V, that
  should be ok because the maximum rating according to the datasheet for
  "All Other Pins to GND" is -0.3V to +6.0V. Adding additional 4.7K 
  pull-ups to 5V without disconnecting the existing 4.7K pull-ups also
  works (the bus high voltage is then 3.4V)

  Note: MAX30102 has IR and red LEDs
        MAX30105 has IR, red and green LEDs
*/
#include "MAX30105.h"           // SparkFun MAX3010x Pulse and Proximity Sensor Library

MAX30105 particleSensor;
long unblockedValue;            // average IR at power up
unsigned long movingAvgHz = 0;  // measurement rate
unsigned long prevMs;           // used to calculate measurement rate

void setup()
{
  Serial.begin(9600);

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) // use default I2C port, 400kHz speed
  {
    Serial.println("MAX3010X was not found. Please check wiring/power.");
    while (true);
  }

  // Setup to sense up to 45 cm, max LED brightness
  const byte ledBrightness = 0xFF;                // options: 0=Off to 255=50mA
  const byte sampleAverage = 4;                   // options: 1, 2, 4, 8, 16, 32
  const byte ledMode = 2;                         // options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  const int sampleRate = 400;                     // options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  const int pulseWidth = 411;                     // options: 69, 118, 215, 411
  const int adcRange = 2048;                      // options: 2048, 4096, 8192, 16384
  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
  particleSensor.setPulseAmplitudeRed(0);         // turn off red LED
  particleSensor.setPulseAmplitudeGreen(0);       // turn off green LED (if available)

  // Take an average of IR readings at power-up
  unblockedValue = 0;
  for (byte x = 0 ; x < 32 ; x++)
    unblockedValue += particleSensor.getIR();
  unblockedValue /= 32;

  // Time
  prevMs = millis();
}

void loop()
{
  unsigned long currentMs = millis();
  unsigned long diffMs = currentMs - prevMs;
  if (diffMs == 0) diffMs = 1; // avoid division by 0
  unsigned long currentHz = 1000 / diffMs;
  prevMs = currentMs;
  movingAvgHz = (3 * movingAvgHz + currentHz) / 4; 
  uint32_t irValue = particleSensor.getIR();
  Serial.print("IR[");
  Serial.print(irValue);
  Serial.print("] Hz[");
  Serial.print(movingAvgHz);
  Serial.print("]");

  long currentDelta = (long)irValue - unblockedValue;

  Serial.print(" delta[");
  Serial.print(currentDelta);
  Serial.print("]");

  if (currentDelta > 500)
    Serial.print(" something is there!");

  Serial.println();
}
