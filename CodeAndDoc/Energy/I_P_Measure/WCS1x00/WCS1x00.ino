/*
  Winson WCS1600, WCS1700 and WCS1800 current measurement through Hall Effect
 
  - VDD supply is 3V – 12V.
  
  - The current to be measured generates a magnetic field which is converted 
    into a voltage, that can be picked-up at the Out pin. Due to its measuring
    principle, the WCS1x00 influences the current to be measured less than a 
    shunt-based module. However the precision is not so good. Even if zero 
    calibrated the sensors are not symmetrical: inverting the current direction
    with the same load returns an absolute value which can also differ by 10%.
  
  - With 5V supply voltage (can be fine tuned with known test currents):
    WCS1800: 66mV/A and max 35A
    WCS1700: 33mV/A and max 70A
    WCS1600: 22mV/A and max 100A
  
  - Vout = VDD/2 at 0A and changes by the specified amount of mV/A
    For example for the WCS1800 sensor:
    Vout = 0.066 * I + VDD/2  ->  I = (Vout - VDD/2) / 0.066​

  - There is a WinsonLib which does what's done here.
*/
#include <PrintCol.h>
 
const byte WCS1x00_PIN = A0;
int zeroCurrent = 512;      // zeroCalibrate() inits that
const float VperA = 0.066;  // set according to the used sensor
const float factorCurrent = 5.0 / (1023.0 * VperA);
const int CALIB_SIZE = 120;
float avgCurrent = 0.0;
#define SERIAL_PLOTTER      false

void zeroCalibrate()
{
  analogRead(WCS1x00_PIN);  // read analog pin to stabilize the ADC
  uint32_t startMicros;
  int32_t dataSum = 0;
  for (int i = 0; i < CALIB_SIZE; i++)
  {
    startMicros = micros();
    dataSum += analogRead(WCS1x00_PIN);   // AVR takes ~100us to read it
    while (micros() - startMicros < 829); // sample rate = 1200Hz
  }
  zeroCurrent = dataSum / CALIB_SIZE;
  Serial.print("zeroCurrent: ");
  Serial.println(zeroCurrent);
}
  
void setup()
{
  Serial.begin(115200);
  Serial.println("WCS1x00 Test");
  zeroCalibrate();
}

void loop()
{
  int rawCurrent = analogRead(WCS1x00_PIN);

  // Instantaneous
  float instCurrent = (rawCurrent - zeroCurrent) * factorCurrent;

  // Moving average
  avgCurrent = (15 * avgCurrent + instCurrent) / 16;
  
#if SERIAL_PLOTTER == true
  Serial.println(instCurrent);
#else
  Serial.print("Instantaneous: ");
  printCol(rawCurrent, DEC, 4);
  Serial.print(" -> ");
  printCol(instCurrent, 2, 6);
  Serial.print("A");
  Serial.print("  |  Average: ");
  printCol(avgCurrent, 2, 6);
  Serial.println("A");
#endif

  delay(50);        
}
