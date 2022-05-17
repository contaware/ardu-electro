/*
  BMP280 temperature and barometric pressure sensor
 
  - VDD supply is 3.3V - 5V.
  - The module has a 3.3V regulator and integrated level-shifters.
*/  
#include <Adafruit_BMP280.h>

Adafruit_BMP280 bmp280;
const bool useForcedMode = false;
const int TIME_BETWEEN_MEASUREMENTS_MS = 1000;
#define BMP280_USED_ADDRESS   0x76  // 0x76 (SDO <-> GND) or 0x77 (library default, SDO <-> VDDIO)

void setup()
{
  Serial.begin(9600);
  Serial.println("BMP280 Test");

  if (!bmp280.begin(BMP280_USED_ADDRESS))
  {
    Serial.println("Could not find a valid BMP280 sensor, check wiring or try a different address!");
    while (true);
  }
  
  /*
    In forced mode the BME sensor goes back to sleep after each measurement; 
    we need to call bmp280.takeForcedMeasurement() to trigger a new measurement.
    In normal mode it does new measurements periodically, use the standby time 
    to regulate the periodicity.
  */
  bmp280.setSampling(useForcedMode ? Adafruit_BMP280::MODE_FORCED : Adafruit_BMP280::MODE_NORMAL,
                    Adafruit_BMP280::SAMPLING_X16,    /* Temp. oversampling */
                    Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                    Adafruit_BMP280::FILTER_OFF,      /* Filtering */
                    Adafruit_BMP280::STANDBY_MS_500); /* Standby time for MODE_NORMAL: 1ms - 4000ms */
}

void loop()
{
  // In forced mode we must call bmp280.takeForcedMeasurement() to wake the sensor up
  // and get a new measurement data, it blocks until the measurement is complete!
  bool doRead = true;
  if (useForcedMode)
    doRead = bmp280.takeForcedMeasurement();

  // Get measurements and print them
  if (doRead)
  {
    float temp  = bmp280.readTemperature();
    float pres  = bmp280.readPressure();
    float alt   = bmp280.readAltitude(1013.25); // pass pressure of the sea level at your location in hPa

    Serial.println("------------------------");
    Serial.print("Temperature = ");
    Serial.print(temp);
    Serial.println(" °C");
    
    Serial.print("Pressure    = ");
    Serial.print(pres / 100);
    Serial.println(" hPa");
    
    Serial.print("Approx. Alt = ");
    Serial.print(alt);
    Serial.println(" m");
  }

  delay(TIME_BETWEEN_MEASUREMENTS_MS);
}
