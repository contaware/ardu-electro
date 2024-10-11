/*
  Si4713 module transmits FM stereo radio and RDS/RBDS messages up to 10m
 
  - Vin supply is 3V - 5V.

  - 3Vo is the output from the onboard regulator: 3.3V and max 100mA.

  - LIN is the line level LEFT input (do feed it with ~0.7 Vpp audio).
    RIN is the line level RIGHT input (do feed it with ~0.7 Vpp audio).
    Both are connected to the stereo headphone jack.
    
  - Connect the RST pin to the digital pin set by RESET_PIN.
  
  - CS is the Chip select pin, used in SPI mode. It also determines the 
    I2C address. When pulled high (it is by default) the I2C address is
    0x63. If this pin is shorted to ground, the I2C address is 0x11.

  - Module has level shifters for SDA and SCL.
  
  - An antenna is required, solder a 1m long wire.

  - By default AGC (automatic gain control) is enabled in 
    Adafruit_Si4713::powerUp() with this line of code:
    setProperty(SI4713_PROP_TX_ACOMP_ENABLE, 0x0);
    To turn-off AGC replace the above mentioned line with:
    setProperty(SI4713_PROP_TX_ACOMP_ENABLE, 0x02);
*/
#include <Adafruit_Si4713.h>

// Change to an unused frequency
#define FM_STATION  10230 // 10230 is 102.30 MHz

// Change to where the RST pin is connected
#define RESET_PIN   12
Adafruit_Si4713 radio = Adafruit_Si4713(RESET_PIN);

// Get frequency in MHz
float getMHz(int freq)
{
  return freq / 100.0;
}

void setup()
{
  // Serial init
  Serial.begin(9600);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!
  Serial.println("Si4713 FM Radio Transmitter Test");

  // Begin
  if (!radio.begin(SI4710_ADDR1)) // SI4710_ADDR1 is 0x63 (default), SI4710_ADDR0 is 0x11
  {
    Serial.println("Could not find the Si4713 FM radio transmitter module!");
    while (true);
  }

  /*
  The Si4713 has the ability to scan the FM band and measure the input power.
  That can be used to locate a good unused station, find a low number that
  is also not surrounded by high numbers.
  */
  for (uint16_t f = 8750; f <= 10800; f+=10) // from 87.5 to 108.0 MHz
  {
    radio.readTuneMeasure(f);
    Serial.print("Measuring "); Serial.print(getMHz(f)); Serial.print(" MHz: ");
    radio.readTuneStatus();
    Serial.print(radio.currNoiseLevel);
    Serial.println(" noise level");
  }

  // Set maximum TX power
  // 88-115 dBμV or 0 for off
  radio.setTXpower(115);

  // Tune into
  radio.tuneFM(FM_STATION);

  // Get the status from the chip
  radio.readTuneStatus();
  Serial.print("Tx Frequency: ");
  Serial.print(getMHz(radio.currFreq));
  Serial.println(" MHz");
  Serial.print("Tx Power: "); 
  Serial.print(radio.currdBuV);
  Serial.println(" dBμV");
  Serial.print("Antenna Tuning Capacitor: "); 
  Serial.println(radio.currAntCap);

  // Send RDS/RDBS message
  radio.beginRDS();
  radio.setRDSstation("MyRadio!");        // station max 8 chars
  radio.setRDSbuffer("Hello World ;-)");  // song name/artist max 32 chars
}

void loop()
{
  // Input signal
  radio.readASQ();
  Serial.print("Audio Signal Quality: 0x"); 
  Serial.println(radio.currASQ, HEX);
  Serial.print("Audio Input Level: "); 
  Serial.print(radio.currInLevel);
  Serial.println(" dBFS");
  
  // Delay
  delay(1000);
}
