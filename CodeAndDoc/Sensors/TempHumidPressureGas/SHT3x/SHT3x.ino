/*
  SHT30, SHT31 and SHT35 humidity and temperature sensor
 
  - VDD chip supply is 2.15 - 5.5V. The chip is directly connected to the
    module's supply. Thus for 5V Arduinos, power the module with 5V and for
    3.3V Arduinos, power it with 3.3V.
    
  - Relative humidity operating range: 0 – 100% RH.

  - Temperature operating range: -40 to +125°C.

  - SHT30 is the least precise, SHT31 is better and SHT35 is the best one.

  - The SHT3x datasheet says that the internal heater is only meant for
    plausibility checking, while Adafruit writes in their guide that it can
    be used to heat/evaporate any condensation:
    https://learn.adafruit.com/adafruit-sht31-d-temperature-and-humidity-sensor-breakout?view=all
    After a reset the heater is disabled (default condition).

  - For the violet module the AD pin has a 10k pull-down that sets the I2C 
    address to 0x44. Connecting this pin to VDD changes the address to 0x45.
    The Wemos D1 mini SHT30 shield has two pads which can be shorted to get 
    the 0x44 address. If left open the address is 0x45.

  - The AL/ALERT pin is an interrupt output. It's possible to set up the 
    sensor to alert when an event has occured (check the datasheet).
*/  
#include "Adafruit_SHT31.h"
#include <PrintCol.h>

Adafruit_SHT31 sht3x = Adafruit_SHT31();
bool enableHeater = false;
unsigned long lastHeaterSwitchMillis;
const int TIME_BETWEEN_MEASUREMENTS_MS = 3000;
#define SHT3x_USED_ADDRESS   0x44  // 0x44 (library default) or 0x45
                          
void setup()
{
  // Init serial
  Serial.begin(9600);
  Serial.println(F("SHT3x Test"));

  // Init sensor
  if (!sht3x.begin(SHT3x_USED_ADDRESS))
  {
    Serial.println(F("Could not find the SHT3x sensor, check wiring or try a different address!"));
    while (true);
  }

  // Check heater state which after a reset should always be disabled
  Serial.print(F("Heater state "));
  if (sht3x.isHeaterEnabled())
    Serial.println(F("ENABLED"));
  else
    Serial.println(F("DISABLED"));
  
  lastHeaterSwitchMillis = millis();
}

void loop()
{
  Serial.println(F("----------------------"));

  // Make measurements
  unsigned long startMillis = millis();
  float t = sht3x.readTemperature();
  float h = sht3x.readHumidity();
  unsigned long endMillis = millis();
  Serial.print(F("Reading needed "));
  Serial.print(endMillis - startMillis);
  Serial.println(F(" ms"));
  
  // Print measurements
  Serial.println(F("       RH %   Temp. °C"));
  if (!isnan(h))
    printCol(h, 1);
  else 
    Serial.print(F("          -"));
  if (!isnan(t))
    printCol(t, 1);
  else 
    Serial.print(F("          -"));
  Serial.println();

  // Toggle heater enabled state every 30 seconds
  // Note: the producer states that a temperature increase of a few
  //       degrees celsius can be observed as a plausibility check.
  unsigned long currentMillis = millis();
  if ((currentMillis - lastHeaterSwitchMillis) > 30000UL)
  {
    lastHeaterSwitchMillis = currentMillis; 
    enableHeater = !enableHeater;
    sht3x.heater(enableHeater);
    Serial.print(F("Heater state "));
    if (sht3x.isHeaterEnabled())
      Serial.println(F("ENABLED"));
    else
      Serial.println(F("DISABLED"));
  }

  delay(TIME_BETWEEN_MEASUREMENTS_MS);
}
