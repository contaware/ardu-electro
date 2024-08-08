/*
  SPH0645 is a mono I2S Microphone, view the audio on the Serial Plotter

  - VDD (3V pin) is 1.6V - 3.6V.

  - The microphone hole is on the bottom side.

  - The data format is 2's complement at 24-bit. The data precision is 
    18-bits, the unused LSB are zero.
  
  - The select channel pin (SEL) is by default low, so that the microphone 
    will transmit on the left channel. Connecting this pin to VDD will 
    instruct the chip to transmit on the right channel.
    It's possible to use two microphones sharing BCLK, LRCLK and DOUT with one 
    having SEL tied to GND and the other to VDD.
    
  - LRCLK (WS/FS) connect to pin 0 (Zero) or pin 3 (MKR) or A2 (Nano 33 IoT).
    BCLK (SCK)    connect to pin 1 (Zero) or pin 2 (MKR) or A3 (Nano 33 IoT).
    DOUT (SD)     connect to pin 9 (Zero) or pin A6 (MKR) or 4 (Nano 33 IoT).
*/

#include <I2S.h>

int avg = 0;
unsigned int goodSamplesCount = 0;

void setup()
{
  // A baud rate of 115200 is used instead of 9600 for a faster data rate
  Serial.begin(115200);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!

  // Start I2S at 8 kHz with 32-bits per sample
  if (!I2S.begin(I2S_PHILIPS_MODE, 8000, 32))
  {
    Serial.println("Failed to initialize I2S!");
    while (true);
  }
}

void loop()
{
  // Read a sample
  int sample = I2S.read();
   
  // Unfortunatelly there is no way to know from which channel a sample comes...
  // we try to filter-out the unused channel like:
  if ((sample != 0) && (sample != -1))
  {
    // Convert to 18-bit signed
    sample >>= 14;

    // Calc. exponentially weighted moving average (EWMA) with n + 1 = 64
    // avg = (sample + n * avg) / (n + 1) = avg + (sample - avg) / (n + 1)
    avg += (sample - avg) >> 6;
    
    // Only print each 40 samples to avoid overflowing the serial port
    if ((goodSamplesCount % 40) == 0)
    {
      Serial.print("h:5000,s:");
      Serial.print(sample - avg); // subtract avg to filter-out the DC bias
      Serial.println(",l:-5000");
    }

    // Inc. samples count
    // Note: if it overflows there is no problem because it is only used above
    //       with the modulo operator.
    goodSamplesCount++;
  }
}
