/*
  SPH0645 is a mono I2S Microphone, view the audio on the Serial Plotter

  - VDD (3V pin) is 1.6V - 3.6V.

  - The microphone hole is on the bottom side.

  - The data format is 2's complement at 24-bit, MSB first. The data precision 
    is 18-bits, the unused LSB are zero.
  
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

  // Show used pins
  Serial.println("I2S started with the following PIN configuration:");
  Serial.print("PIN_I2S_FS="); Serial.println(PIN_I2S_FS);
  Serial.print("PIN_I2S_SCK="); Serial.println(PIN_I2S_SCK);
  #if defined(PIN_I2S_SDI) && defined(PIN_I2S_SDO)
    Serial.print("PIN_I2S_SDO="); Serial.println(PIN_I2S_SDO);
    Serial.print("PIN_I2S_SDI="); Serial.println(PIN_I2S_SDI)
  #else
    Serial.print("PIN_I2S_SD="); Serial.println(PIN_I2S_SD);
  #endif
}

void loop()
{
  // Read a sample
  int sample = I2S.read();
   
  // Unfortunatelly there is no way to know from which channel a sample comes...
  // we try to filter-out the unused channel like:
  if ((sample != 0) && (sample != -1))
  {
    sample >>= 14; // convert to 18-bit signed
    Serial.println(sample); // there is a DC offset which can be removed by subtracting the samples average
  }
}
