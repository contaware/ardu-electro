/*
  SPH0645 is a mono I2S Microphone, view the audio on the Serial Plotter

  - VDD (3V pin) is 1.6V - 3.6V.

  - The microphone hole is on the bottom side.

  - The data format is 2's complement at 24-bit. The data precision is 
    18-bits, the unused LSB are zero. The returned data have a DC offset.
  
  - When the select channel pin (SEL) is low, the microphone will transmit on
    the left channel. Connecting this pin to VDD will instruct the chip to 
    transmit on the right channel.
    It's possible to use two microphones by sharing BCLK, LRCL and DOUT; 
    one would have SEL tied to GND and the other one to VDD.
    
  - LRCL (WS/FS) connect to pin 0 (Zero) or pin 3 (MKR) or A2 (Nano 33 IoT) or D8 (Nano ESP32).
    BCLK (SCK)   connect to pin 1 (Zero) or pin 2 (MKR) or A3 (Nano 33 IoT) or D7 (Nano ESP32).
    DOUT (SD)    connect to pin 9 (Zero) or pin A6 (MKR) or 4 (Nano 33 IoT) or D9 (Nano ESP32).
*/

#include <I2S.h>

int avgLeft = 0;
int avgRight = 0;
unsigned int samplePairsCount = 0;

void setup()
{
  // A baud rate of 115200 is used instead of 9600 for a faster data rate
  Serial.begin(115200);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!
  delay(5000);      // for ESP32 and some other MCUs a delay() is needed, otherwise
                    // the messages generated in setup() can't be seen!

  // Start I2S at 8 kHz with 32-bits per sample
  if (!I2S.begin(I2S_PHILIPS_MODE, 8000, 32))
  {
    Serial.println("Failed to initialize I2S!");
    while (true);
  }
}

void loop()
{
  // Do we have at least two samples (left & right)?
  if (I2S.available() > 1)
  {
    // Read channels
    // Note: depending from the platform left and right may be swapped.
    int sampleLeft = I2S.read();
    sampleLeft >>= 14;  // convert to 18-bit signed
    int sampleRight = I2S.read();
    sampleRight >>= 14; // convert to 18-bit signed

    // Calc. exponentially weighted moving average (EWMA) with n + 1 = 64
    // avg = (sample + n * avg) / (n + 1) = avg + (sample - avg) / (n + 1)
    avgLeft += (sampleLeft - avgLeft) >> 6;
    avgRight += (sampleRight - avgRight) >> 6;

    // Only print each 64 samples to avoid overflowing the serial port
    if ((samplePairsCount % 64) == 0)
    {
      Serial.print("T:6000,R:");
      Serial.print(sampleRight - avgRight); // subtract avg to filter-out the DC offset
      Serial.print(",L:");
      Serial.print(sampleLeft - avgLeft);   // subtract avg to filter-out the DC offset
      Serial.println(",B:-6000");
    }

    // Inc. sample pairs count
    // Note: if it overflows there is no problem because it is only used above
    //       with the modulo operator.
    samplePairsCount++;
  }
}
