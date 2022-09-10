/*
  BME280 humidity, temperature and barometric pressure sensor
 
  - VDD supply is 3.3V - 5V.
  
  - The module has a 3.3V regulator and integrated level-shifters.
*/  
#include <Adafruit_BME280.h>
#include <PrintCol.h>

Adafruit_BME280 bme;
const bool useForcedMode = false;
const int TIME_BETWEEN_MEASUREMENTS_MS = 1000;
#define BME280_USED_ADDRESS   0x76  // 0x76 (SDO <-> GND) or 0x77 (library default, SDO <-> VDDIO)

void setup()
{
  Serial.begin(9600);
  Serial.println("BME280 Test");

  if (!bme.begin(BME280_USED_ADDRESS))
  {
    Serial.println("Could not find the BME280 sensor, check wiring or try a different address!");
    while (true);
  }
  
  /*
    In forced mode the BME sensor goes back to sleep after each measurement; 
    we need to call bme.takeForcedMeasurement() to trigger a new measurement.
    In normal mode it does new measurements periodically, use the standby time 
    to regulate the periodicity.
  */
  bme.setSampling(useForcedMode ? Adafruit_BME280::MODE_FORCED : Adafruit_BME280::MODE_NORMAL,
                  Adafruit_BME280::SAMPLING_X16,    /* Temp. oversampling */
                  Adafruit_BME280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BME280::SAMPLING_X16,    /* Humidity oversampling */
                  Adafruit_BME280::FILTER_OFF,      /* Filtering */
                  Adafruit_BME280::STANDBY_MS_500); /* Standby time for MODE_NORMAL: 0.5ms - 1000ms */
}

void loop()
{
  // In forced mode we must call bme.takeForcedMeasurement() to wake the sensor up
  // and get a new measurement data, it blocks until the measurement is complete!
  bool doRead = true;
  if (useForcedMode)
    doRead = bme.takeForcedMeasurement();

  // Get measurements and print them
  if (doRead)
  {
    Serial.println("--------------------------------------------");
    Serial.println(" Humidity %   Temp. °C  Pres. hPa     Alt. m");
    printCol(bme.readHumidity());
    printCol(bme.readTemperature());
    printCol(bme.readPressure() / 100.0);
    printCol(bme.readAltitude(1013.25)); // pass the pressure of the sea level at your location in hPa
    Serial.println();
  }

  delay(TIME_BETWEEN_MEASUREMENTS_MS);
}
