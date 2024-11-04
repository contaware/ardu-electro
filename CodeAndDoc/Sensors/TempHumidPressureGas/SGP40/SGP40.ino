/*
  SGP40 Indoor Air Quality Sensor for VOC Measurements
 
  - VDD supply is 3.3V - 5V.
  - The module has a 3.3V regulator and integrated level-shifters.
  - The SGP40 supports only one I2C address which is 0x59.
  - After each power-on it may take around a minute for the Voc index
    to start changing as it calibrates the baseline readings. In this 
    phase a voc index of 0 is returned.
  - Voc index can directly indicate the condition of air quality:
    0-100   no need to ventilate
    100-200 no need to ventilate
    200-400 ventilate
    400-500 ventilate intensely!
*/
#include "Adafruit_SGP40.h"

Adafruit_SGP40 sgp;

void setup()
{
  Serial.begin(9600);
  Serial.println("SGP40 test");
  if (!sgp.begin())
  {
    Serial.println("SGP40 sensor not found");
    while (true);
  }
  Serial.print("Found SGP40 serial #");
  Serial.print(sgp.serialnumber[0], HEX);
  Serial.print(sgp.serialnumber[1], HEX);
  Serial.println(sgp.serialnumber[2], HEX);
}

void loop()
{
  // To increase accuracy read in from another chip:
  //float t; // in °C
  //float h; // relative humidity in % rH
  //int32_t voc_index = sgp.measureVocIndex(t, h);
  int32_t voc_index = sgp.measureVocIndex();
  Serial.print("Voc Index: ");
  Serial.println(voc_index);
  
  delay(1000);
}
