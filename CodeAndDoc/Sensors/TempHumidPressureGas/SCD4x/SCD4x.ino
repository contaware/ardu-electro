/*
  SCD4x photoacoustic true CO₂, temperature and humidity sensor
 
  - VDD supply is 3.3V - 5V.
  
  - The SCD4x supports only one I2C address which is 0x62.
    
  - The SCD-40 is lower cost, and is perfect for indoor/outdoor air quality
    and CO₂ measurements. It has a range of 400-2000 ppm with an accuracy
    of ±(50 ppm + 5% of reading). typical consumption of 15mA at 3.3V or 
    11mA at 5V.
    
  - The SCD-41 is more expensive, and while it can definitely be used for 
    air quality, it's wide range means its best used for industrial or 
    scientific CO₂ measurements where the ppm can get very high. It has a
    range of 400-5000 ppm with an accuracy of ±(40 ppm + 5% of reading).
    Typical consumption of only 0.45mA at 3.3V and 0.36mA at 5V, which 
    allows us to create CO₂ meters with great autonomy. To achieve this very
    low consumption, the SCD41 has a "single measure" operating mode, called 
    "Single shot mode" in which the sensor is used on request to take a single
    measurement (the previous consumption corresponds to one measurement every
    5 minutes).
*/
#include <SensirionI2CScd4x.h> // Sensirion I2C SCD4x

SensirionI2CScd4x scd4x;
uint16_t error;
char errorMessage[256];
  
void printUint16Hex(uint16_t value)
{
  Serial.print(value < 4096 ? "0" : "");
  Serial.print(value < 256 ? "0" : "");
  Serial.print(value < 16 ? "0" : "");
  Serial.print(value, HEX);
}

void printSerialNumber(uint16_t serial0, uint16_t serial1, uint16_t serial2)
{
  Serial.print("Serial: 0x");
  printUint16Hex(serial0);
  printUint16Hex(serial1);
  printUint16Hex(serial2);
  Serial.println();
}

void setup()
{
  // Begin serial
  Serial.begin(9600);
  while (!Serial)
    delay(100);

  // Begin I2C
  Wire.begin();

  // Begin SCD4x
  scd4x.begin(Wire);

  // Stop potentially previously started measurement
  error = scd4x.stopPeriodicMeasurement();
  if (error)
  {
    Serial.print("Error trying to execute stopPeriodicMeasurement(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
  }

  // Print serial num
  uint16_t serial0;
  uint16_t serial1;
  uint16_t serial2;
  error = scd4x.getSerialNumber(serial0, serial1, serial2);
  if (error)
  {
    Serial.print("Error trying to execute getSerialNumber(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
  }
  else
    printSerialNumber(serial0, serial1, serial2);

  // Start measurement
  error = scd4x.startPeriodicMeasurement();
  if (error)
  {
    Serial.print("Error trying to execute startPeriodicMeasurement(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
  }

  // Tell user to wait 5 sec
  Serial.println("Waiting for first measurement... (5 sec)");
}

void loop()
{
  // Wait 5 sec
  delay(5000);

  // Read Measurement
  uint16_t co2;
  float temperature;
  float humidity;
  error = scd4x.readMeasurement(co2, temperature, humidity);
  if (error)
  {
    Serial.print("Error trying to execute readMeasurement(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
  }
  else if (co2 == 0)
    Serial.println("Invalid sample detected, skipping.");
  else
  {
    Serial.print("CO₂: ");
    Serial.print(co2);
    Serial.print("ppm  ");
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print("°C  ");
    Serial.print("Relative humidity: ");
    Serial.print(humidity);
    Serial.println("%");
  }
}
