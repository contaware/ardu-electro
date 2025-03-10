/*
  BMP085 / BMP180 temperature and barometric pressure sensor
 
  - VDD supply is 3.3V - 5V.
  
  - The module has a 3.3V regulator and integrated level-shifters.
  
  - The BMP085 has been discontinued by BOSCH and replaced with the
    BMP180 which is smaller and less expensive but is identical in 
    terms of wiring and software. Both use the I2C address 0x77.
*/  
#include <Adafruit_BMP085.h> // Adafruit BMP085 Library
#include <PrintCol.h>

Adafruit_BMP085 bmp;
const int TIME_BETWEEN_MEASUREMENTS_MS = 1000;

void setup()
{
  // Init Serial (leave Serial Monitor open to see all messages)
  Serial.begin(9600); delay(5000); // wait 5s that Serial is ready
  Serial.println("BMP085 / BMP180 Test");
  
  // Init BMP085 / BMP180
  if (!bmp.begin())
  {
    Serial.println("Could not find the BMP085 / BMP180 sensor, check wiring or try a different address!");
    while (true);
  }
}

void loop()
{
  Serial.println("---------------------------------");
  Serial.println("   Temp. °C  Pres. hPa     Alt. m");
  printCol(bmp.readTemperature());
  printCol(bmp.readPressure() / 100.0);
  printCol(bmp.readAltitude(101325)); // pass the pressure of the sea level at your location in Pa
  Serial.println();
  delay(TIME_BETWEEN_MEASUREMENTS_MS);
}
