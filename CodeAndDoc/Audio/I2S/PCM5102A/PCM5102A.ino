/*
  PCM5102A is a stereo audio DAC with I2S audio input

  - Some modules can be powered by 5V because they have LDOs, but the MCU must
    output 3.3V signals for the 3.3V PCM5102A chip.

  - The chip has 4 configuration pins:
    FLT Filter selection:       GND = Normal latency (default), 3.3V = Low latency
    DEMP De-emphasis control:   GND = OFF (default),            3.3V = ON 
    XSMT Soft mute control:     GND = Mute,                     3.3V = Un-Mute (default) 
    FMT Audio format selection: GND = I2S (default),            3.3V = Left justified 
    
  - VIN         connect to 3.3V or 5V if the module has LDOs.
    GND         connect to GND.
    SCK         connect to GND to reduce noise.
    LCK (WS/FS) connect to pin D0 (Zero) or D3 (MKR) or A2 (Nano 33 IoT) or D8 (Nano ESP32).
    BCK         connect to pin D1 (Zero) or D2 (MKR) or A3 (Nano 33 IoT) or D7 (Nano ESP32).
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
  // Init serial
  Serial.begin(9600);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!
  delay(5000);      // for ESP32 and some other MCUs a delay() is needed, otherwise
                    // the messages generated in setup() can't be seen!

  Serial.println("PCM5102A Test");

  // Init sine waves
  for (int i=0; i < BUFSIZE; i++)
  {
    // Create a sine wave on the left channel (8000 / BUFSIZE = 250 Hz)
    left[i] = sin( (2*PI / (BUFSIZE) ) * i) * VOLUME;
    
    // Double the frequency and reduce the volume for the right channel
    right[i] = sin( 2 * (2*PI / (BUFSIZE) ) * i) * VOLUME / 3;
  }

  // Start I2S with 32-bit depth and at 8000 samples per second
  if (!I2S.begin(I2S_PHILIPS_MODE, 8000, 32)) {
    Serial.println("Failed to initialize I2S!");
    while (true);
  }
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
