/*
  MAX30102 or MAX30105 modules
  
  - VDD supply is 3.3V - 5.5V.
  - The chip runs at 1.8V, for the I2C interface some modules have level 
    shifters built-in, others have pull-ups to 1.8V or 3.3V.
    For the modules with 1.8V pull-ups some people suggest to disconnect
    the module pull-ups and do external pull-ups to 3.3V or 5V, that
    should be ok because the maximum rating according to the datasheet for
    "All Other Pins to GND" is -0.3V to +6.0V. Adding additional 4.7K 
    pull-ups to 5V without disconnecting the existing 4.7K pull-ups also
    works (the bus high voltage is then 3.4V).
  - MAX30102 has IR and red LEDs, MAX30105 has IR, red and green LEDs.
*/
#include "MAX30105.h"         // SparkFun MAX3010x Pulse and Proximity Sensor Library
#include "heartRate.h"

MAX30105 particleSensor;
const byte RATE_SIZE = 4;     // increase this for more averaging, 4 is a good value
byte rates[RATE_SIZE] = {};   // array of heart rates
byte rateSpot = 0;
unsigned long lastBeatMs;     // time at which the last beat occurred

void setup()
{
  Serial.begin(9600);
  
  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) // use default I2C port, 400kHz speed
  {
    Serial.println("MAX3010X was not found. Please check wiring/power.");
    while (true);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  // Setup
  particleSensor.setup();                     // configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A);  // set red LED low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0);   // turn off the green LED

  // Time
  lastBeatMs = millis();
}

void loop()
{
  uint32_t irValue = particleSensor.getIR();

  // Did we sense a beat?
  if (checkForBeat(irValue))
  {
    // Current beats per minute
    unsigned currentBeatMs = millis();
    unsigned long deltaMs = currentBeatMs - lastBeatMs;
    if (deltaMs == 0) deltaMs = 1; // avoid division by 0
    lastBeatMs = currentBeatMs;
    unsigned long beatsPerMinute = 60 * 1000UL / deltaMs;

    // Do a avg of the last RATE_SIZE beats per minute
    unsigned int beatsPerMinuteAvg = 0;
    if (beatsPerMinute >= 10 && beatsPerMinute <= 255)
    {
      // Store this reading in the array
      rates[rateSpot++] = (byte)beatsPerMinute;
      rateSpot %= RATE_SIZE; // wrap variable

      // Take average of readings
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatsPerMinuteAvg += rates[x];
      beatsPerMinuteAvg /= RATE_SIZE;

      // Print
      Serial.print("IR=");
      Serial.print(irValue);
      Serial.print(", BPM=");
      Serial.print(beatsPerMinute);
      Serial.print(", Avg BPM=");
      Serial.println(beatsPerMinuteAvg);
    }
  }

  // Alert?
  if (irValue < 50000)
    Serial.println("No finger?");
}
