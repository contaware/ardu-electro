/*
  ADS1115 16-bit ADC module with 4 channels
 
  - VDD supply is 2.0V - 5.5V.

  - Although the ADS1115 has 16-bit resolution capability, it uses twos 
    complement to represent a value. Thus it is a 15-bit ADC capable of 
    negative and positive voltage measurements. The real use of the 
    ±15bit resolution is when you use differential mode.
*/
#include <Adafruit_ADS1X15.h>
 
Adafruit_ADS1115 ads; // use Adafruit_ADS1015 for the 12-bit version
 
void setup() 
{
  Serial.begin(9600);

  Serial.println("Getting single-ended readings from AIN0..3");

  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful to never exceed VDD + 0.3V, or to exceed 
  // the upper and lower limits if you adjust the input range!
  //
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV

  if (!ads.begin())
  {
    Serial.println("Failed to initialize ADS.");
    while (true);
  }
}
 
void loop() 
{
  int16_t adc0, adc1, adc2, adc3;
  float volts0, volts1, volts2, volts3;

  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  adc2 = ads.readADC_SingleEnded(2);
  adc3 = ads.readADC_SingleEnded(3);

  volts0 = ads.computeVolts(adc0);
  volts1 = ads.computeVolts(adc1);
  volts2 = ads.computeVolts(adc2);
  volts3 = ads.computeVolts(adc3);

  Serial.println("-----------------------------------------------------------");
  Serial.print("AIN0: "); Serial.print(adc0); Serial.print("  "); Serial.print(volts0, 3); Serial.println("V");
  Serial.print("AIN1: "); Serial.print(adc1); Serial.print("  "); Serial.print(volts1, 3); Serial.println("V");
  Serial.print("AIN2: "); Serial.print(adc2); Serial.print("  "); Serial.print(volts2, 3); Serial.println("V");
  Serial.print("AIN3: "); Serial.print(adc3); Serial.print("  "); Serial.print(volts3, 3); Serial.println("V");

  delay(1000);
}
