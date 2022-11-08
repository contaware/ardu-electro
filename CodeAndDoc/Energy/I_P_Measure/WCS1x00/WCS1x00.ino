/*
  WCS1600, WCS1700 and WCS1800 current measurement through Hall Effect
 
  - VDD supply is 3V – 12V.
  
  - In the WCS1x00 the current to be measured generates a magnetic field
    which is converted into a voltage, that can be picked-up at the Out pin.
    Due to its measuring principle, the WCS1x00 influences the current to be
    measured less than a shunt-based module. However the precision is not
    so good.
  
  - 3 sensors:
    WCS1800: 66mV/A and max 35A
    WCS1700: 33mV/A and max 70A
    WCS1600: 22mV/A and max 100A
  
  - Vout = VDD/2 at 0A and changes by the specified amount of mV/A
    For example for the WCS1800 sensor:
    Vout = 0.066 * I + VDD/2  ->  I = (Vout - VDD/2) / 0.066​
*/
#include <PrintCol.h>
 
const byte WCS1x00_PIN = A0;
const int zeroCurrent = 533;  // test that!
const float VperA = 0.066;    // set that according to the used sensor
const float factor = 5.0 / (1023.0 * VperA);
float currentAvg = 0.0;
#define SERIAL_PLOTTER   false

void setup()
{
  Serial.begin(115200);
  Serial.println("WCS1x00 Test");
  Serial.println();
}

void loop()
{
  int raw = analogRead(WCS1x00_PIN);

  // Instantaneous
  //    current = (raw - zeroCurrent) * 5.0 / 1023 / VperA;
  float current = (raw - zeroCurrent) * factor;

  // Moving average
  currentAvg = (127 * currentAvg + current) / 128;
  
#if SERIAL_PLOTTER == true
  Serial.println(current);
#else
  Serial.print("Instantaneous: ");
  printCol(raw, DEC, 4);
  Serial.print(" -> ");
  printCol(current, 2, 6);
  Serial.print("A");
  Serial.print("  |  Average: ");
  printCol(currentAvg, 2, 6);
  Serial.println("A");
#endif

  delay(30);        
}
