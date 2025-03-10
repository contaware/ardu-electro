/*
  MAX98357 is a mono audio amp with I2S audio input

  - Vin supply is 2.7V - 5.5V (the chip itself supports this wide range).
    Note: also if powered by 5V, the logic signal levels can be 3.3V.

  - Output power when Vin is 5V: 3.2W at 4Ω and 1.8W at 8Ω.

  - GAIN pin:
    * 15dB if a 100kΩ resistor is connected between GAIN and GND.
    * 12dB if GAIN is connected directly to GND.
    * 9dB if GAIN is not connected to anything (default).
    * 6dB if GAIN is connected directly to Vin.
    * 3dB if a 100kΩ resistor is connected between GAIN and Vin.

  - ShutDown/Mode SD pin (do not confuse with the DIN pin which is also 
    called SD):
    * If connected to GND then the amp is shutdown.
    * If between 0.16V and 0.77V then the output is (L+R)/2
      Note: that mode is the module default for a Vin of 5V and usually 
            it works also for a Vin of 3.3V, but in case of problems, 
            lower the 1MΩ resistor to 630kΩ (value from the datasheet 
            formula for a Vin of 3.3V).
    * If between 0.77V and 1.4V then the output is R.
    * If higher than 1.4V then the output is L.
    It's possible to use two amps by sharing BCLK, LRC and DIN; one would 
    have the ShutDown/Mode SD pin set for the L channel and the other one 
    for the R channel.
    
  - Vin         connect to 5V or 3.3V.
    GND         connect to GND.
    LRC (WS/FS) connect to pin D0 (Zero) or D3 (MKR) or A2 (Nano 33 IoT) or D8 (Nano ESP32).
    BCLK (SCK)  connect to pin D1 (Zero) or D2 (MKR) or A3 (Nano 33 IoT) or D7 (Nano ESP32).
    DIN (SD)    connect to pin D9 (Zero) or A6 (MKR) or D4 (Nano 33 IoT) or D9 (Nano ESP32).
*/

#include <I2S.h>

// Maximum volume for 32 bit data
#define VOLUME ( (1UL << 31) - 1)

// Create buffers for left and right channels
#define BUFSIZE 32
int left[BUFSIZE];
int right[BUFSIZE];

void setup()
{
  // Init sine waves
  for (int i=0; i < BUFSIZE; i++)
  {
    // Create a sine wave on the left channel (8000 / BUFSIZE = 250 Hz)
    left[i] = sin( (2*PI / (BUFSIZE) ) * i) * VOLUME;
    
    // Double the frequency and reduce the volume for the right channel
    right[i] = sin( 2 * (2*PI / (BUFSIZE) ) * i) * VOLUME / 3;
  }

  // Start I2S with 32-bit depth and at 8000 samples per second
  if (!I2S.begin(I2S_PHILIPS_MODE, 8000, 32))
    while (true);
}

void loop()
{
  // Write output buffers
  // Note: I2S.write() will block until written.
  for (int i=0; i < BUFSIZE; i++)
  {
#if defined(ARDUINO_ARCH_SAMD)
    I2S.write(right[i]);
    I2S.write(left[i]);
#else /* Nano ESP32 */
    I2S.write(left[i]);
    I2S.write(right[i]);
#endif
  }
}
