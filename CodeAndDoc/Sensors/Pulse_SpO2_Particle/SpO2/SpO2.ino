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
#include "spo2_algorithm.h"

MAX30105 particleSensor;

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
// Arduino Uno doesn't have enough SRAM to store 100 samples of IR led data and red led data in 32-bit format
// To solve this problem, 16-bit MSB of the sampled data will be truncated. Samples become 16-bit data.
uint16_t irBuffer[100];   // ir LED sensor data
uint16_t redBuffer[100];  // red LED sensor data
#else
uint32_t irBuffer[100];   // ir LED sensor data
uint32_t redBuffer[100];  // red LED sensor data
#endif

int32_t bufferLength;     // data length
int32_t spo2;             // SPO2 value
int8_t validSPO2;         // indicator to show if the SPO2 calculation is valid
int32_t heartRate;        // heart rate value
int8_t validHeartRate;    // indicator to show if the heart rate calculation is valid

byte pulseLED = 11;       // must be on PWM pin
byte readLED = 13;        // blinks with each data read

void setup()
{
  Serial.begin(9600);

  pinMode(pulseLED, OUTPUT);
  pinMode(readLED, OUTPUT);
  
  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) // use default I2C port, 400kHz speed
  {
    Serial.println(F("MAX3010X was not found. Please check wiring/power."));
    while (true);
  }
  Serial.println(F("Attach sensor to finger with rubber band. Press any key to start conversion"));
  while (Serial.available() == 0) ; // wait until user presses a key
  Serial.read();

  // Setup
  const byte ledBrightness = 60;  // options: 0=Off to 255=50mA
  const byte sampleAverage = 4;   // options: 1, 2, 4, 8, 16, 32
  const byte ledMode = 2;         // options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  const byte sampleRate = 100;    // options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  const int pulseWidth = 411;     // options: 69, 118, 215, 411
  const int adcRange = 4096;      // options: 2048, 4096, 8192, 16384
  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
}

void loop()
{
  bufferLength = 100; // buffer length of 100 stores 4 seconds of samples running at 25sps

  // Read the first 100 samples, and determine the signal range
  for (byte i = 0 ; i < bufferLength ; i++)
  {
    while (particleSensor.available() == false) // do we have new data?
      particleSensor.check(); // check the sensor for new data

    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample(); // we're finished with this sample so move to next sample

    Serial.print(F("red="));
    Serial.print(redBuffer[i], DEC);
    Serial.print(F(", ir="));
    Serial.println(irBuffer[i], DEC);
  }

  // Calculate heart rate and SpO2 after first 100 samples (first 4 seconds of samples)
  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);

  // Continuously taking samples from MAX3010X, heart rate and SpO2 are calculated every 1 second
  while (true)
  {
    // Dumping the first 25 sets of samples in the memory and shift the last 75 sets of samples to the top
    for (byte i = 25; i < 100; i++)
    {
      redBuffer[i - 25] = redBuffer[i];
      irBuffer[i - 25] = irBuffer[i];
    }

    // Take 25 sets of samples before calculating the heart rate.
    for (byte i = 75; i < 100; i++)
    {
      while (particleSensor.available() == false) // do we have new data?
        particleSensor.check(); // check the sensor for new data

      digitalWrite(readLED, !digitalRead(readLED)); // blink onboard LED with every data read

      redBuffer[i] = particleSensor.getRed();
      irBuffer[i] = particleSensor.getIR();
      particleSensor.nextSample(); // we're finished with this sample so move to next sample

      // Send samples and calculation result to terminal program through UART
      Serial.print(F("red="));
      Serial.print(redBuffer[i], DEC);
      Serial.print(F(", ir="));
      Serial.print(irBuffer[i], DEC);

      Serial.print(F(", HR="));
      Serial.print(heartRate, DEC);

      Serial.print(F(", HRvalid="));
      Serial.print(validHeartRate, DEC);

      Serial.print(F(", SPO2="));
      Serial.print(spo2, DEC);

      Serial.print(F(", SPO2Valid="));
      Serial.println(validSPO2, DEC);
    }

    // After gathering 25 new samples recalculate HR and SP02
    maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
  }
}
