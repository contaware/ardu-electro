/*
  INMP441 is a mono I2S Microphone, view the audio on the Serial Plotter

  - VDD (3V pin) is 1.6V - 3.6V.

  - The microphone hole is on the bottom side.

  - The data format is 2's complement at 24-bit.

  - When the select channel pin (L/R) is low, the microphone will 
    transmit on the left channel. Connecting this pin to VDD will 
    instruct the chip to transmit on the right channel. 
    It's possible to use two microphones by sharing SCK, WS and SD; 
    one would have L/R tied to GND and the other one to VDD.
    
  - WS/FS connect to pin D0 (Zero) or D3 (MKR) or A2 (Nano 33 IoT) or D8 (Nano ESP32).
    SCK   connect to pin D1 (Zero) or D2 (MKR) or A3 (Nano 33 IoT) or D7 (Nano ESP32).
    SD    connect to pin D9 (Zero) or A6 (MKR) or D4 (Nano 33 IoT) or D9 (Nano ESP32).
*/

#include <I2S.h>

const int wantedBitsPerSample = 32; // 8, 16 or 32
const int bytesPerSamplePair = 2 * (wantedBitsPerSample / 8);
unsigned int samplePairsCount = 0;

void setup()
{
  // Init Serial
  // A baud rate of 115200 is used instead of 9600 for a faster data rate
  Serial.begin(115200);

  // Start I2S at 8 kHz with the given bits per sample
  if (!I2S.begin(I2S_PHILIPS_MODE, 8000, wantedBitsPerSample))
    while (true);
}

void loop()
{
  // Do we have a left sample and a right sample?
  if (I2S.available() >= bytesPerSamplePair)
  {
    // Read channels
#if defined(ARDUINO_ARCH_SAMD)
    int sampleRight = I2S.read();
    int sampleLeft = I2S.read();
#else /* Nano ESP32 */
    int sampleLeft = I2S.read();
    int sampleRight = I2S.read();
#endif

    // Convert to 24-bit signed
    sampleLeft >>= 8;
    sampleRight >>= 8;

    // Only print each 64 sample pairs to avoid overflowing the serial port
    if ((samplePairsCount % 64) == 0)
    {
      Serial.print("T:400000,R:");
      Serial.print(sampleRight);
      Serial.print(",L:");
      Serial.print(sampleLeft);
      Serial.println(",B:-400000");
    }

    // Inc. sample pairs count
    // Note: if it overflows there is no problem because it is only used above
    //       with the modulo operator.
    samplePairsCount++;
  }
}
