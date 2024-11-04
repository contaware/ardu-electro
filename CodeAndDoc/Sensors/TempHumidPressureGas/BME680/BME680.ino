/*
  BME680 gas, humidity, temperature and barometric pressure sensor
 
  - VDD supply is 3.3V - 5V.
  
  - The module has a 3.3V regulator and integrated level-shifters.

  - The SDO pin determines the I2C address of the module. This pin
    has a built-in pull-up resistor. Therefore, when you leave the
    SDO pin unconnected, the default I2C address is 0x77 and when 
    you connect it to GND, the line is pulled LOW and the I2C 
    address becomes 0x76.
  
  - The BME680 contains a small MOX sensor which changes resistance 
    based on the volatile organic compounds (VOC) in the air. This 
    value takes up to 30 minutes to stabilize, higher concentrations
    of VOC will make the resistance lower. The BME680 doesn't have 
    built-in air quality calculation capabilities like other sensors.
    There is a BOSCH library which converts between gas resistance and
    VOC air quality index, search for "BSEC Software Library" in
    Library Manager (works well with SAMD21 or ESP8266/ESP32 boards):
    https://github.com/boschsensortec/BSEC-Arduino-library
 
  - The BME680 is well known for having high temperature readings 
    due to self-heating (around 1-2°C above the room temperature).
    The BME680 reads the temperature more for itself so that it can 
    compensate its humidity and pressure readings. Some users have
    reported that repeated readings at short intervals do accumulate
    heat, so that the temperature offset increases even more.
*/  
#include <Adafruit_BME680.h>
#include <PrintCol.h>

Adafruit_BME680 bme;
const bool useAsyncMode = false;
const int TIME_BETWEEN_MEASUREMENTS_MS = 3000;
#define BME680_USED_ADDRESS   0x77  // 0x76 (SDO <-> GND) or 0x77 (library default, SDO <-> VDDIO)
                          
void setup()
{
  Serial.begin(9600);
  Serial.println("BME680 Test");

  if (!bme.begin(BME680_USED_ADDRESS))
  {
    Serial.println("Could not find the BME680 sensor, check wiring or try a different address!");
    while (true);
  }
  
  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320°C for 150 ms
}

float calcAltitude(float seaLevel)
{
  float atmospheric = bme.pressure / 100.0F;
  return 44330.0 * (1.0 - pow(atmospheric / seaLevel, 0.1903));
}

void loop()
{
  bool readOK;
  unsigned long startMillis, endMillis;

  Serial.println("-------------------------------------------------------");

  // Make measurements
  if (useAsyncMode)
  {
    // Start the reading
    startMillis = millis();
    unsigned long endTime = bme.beginReading();
    endMillis = millis();
    Serial.print(F("beginReading() needed "));
    Serial.print(endMillis - startMillis);
    Serial.println(F(" ms"));
    Serial.print(F("reading is expected to take "));
    Serial.print(endTime - startMillis);
    Serial.println(F(" ms"));
    
    // Do some work
    delay(100); // this represents parallel work
    
    // Obtain measurement results from BME680
    // Note: there's no need to delay() until millis() >= endTime,
    //       bme.endReading() blocks until done.
    startMillis = millis();
    readOK = bme.endReading();
    endMillis = millis();
    Serial.print(F("endReading() needed "));
    Serial.print(endMillis - startMillis);
    Serial.println(F(" ms"));
  }
  else
  {
    startMillis = millis();
    readOK = bme.performReading();
    endMillis = millis();
    Serial.print(F("performReading() needed "));
    Serial.print(endMillis - startMillis);
    Serial.println(F(" ms"));
  }
  
  // Print measurements
  if (readOK)
  {
    Serial.println("     Gas kΩ       RH %   Temp. °C  Pres. hPa     Alt. m");
    printCol(bme.gas_resistance / 1000.0);
    printCol(bme.humidity);
    printCol(bme.temperature);
    printCol(bme.pressure / 100.0);
    printCol(calcAltitude(1013.25)); // pass the pressure of the sea level at your location in hPa
    Serial.println();
  }

  delay(TIME_BETWEEN_MEASUREMENTS_MS);
}
