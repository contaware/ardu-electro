/*
  SCD4x photoacoustic true CO₂, temperature and humidity sensor
 
  - VDD supply is 3.3V - 5V.
  
  - The SCD4x supports only one I2C address which is 0x62.
    
  - The SCD-40 is lower cost, and is perfect for indoor/outdoor air 
    quality and CO₂ measurements. It has a range of 400-2000ppm with an 
    accuracy of ±(50ppm + 5% of reading). Typical consumption of 15mA 
    at 3.3V or 11mA at 5V.
    
  - The SCD-41 is more expensive, and while it can definitely be used 
    for air quality, it's wide range means its best used for industrial 
    or scientific CO₂ measurements where the ppm can get very high. It 
    has a range of 400-5000ppm with an accuracy of ±(40ppm + 5% of 
    reading). Typical consumption of only 0.45mA at 3.3V and 0.36mA at 
    5V, which allows us to create CO₂ meters with great autonomy. To 
    achieve this very low consumption, the SCD41 has a "single measure" 
    operating mode, called "Single shot mode" in which the sensor is 
    used on request to take a single measurement (the mentioned 
    consumption corresponds to one measurement every 5 minutes).
*/
#include <SensirionI2cScd4x.h> // Sensirion I2C SCD4x

SensirionI2cScd4x scd4x;
int16_t error;
char errorMessage[64];

#ifdef NO_ERROR
#undef NO_ERROR
#endif
#define NO_ERROR 0

void setup()
{
  // Init Serial (leave Serial Monitor open to see all messages)
  Serial.begin(9600); delay(5000); // wait 5s that Serial is ready
  Serial.println("SCD4x Test");

  // Init I2C
  Wire.begin();

  // Init SCD4x
  scd4x.begin(Wire, SCD41_I2C_ADDR_62);

  // Stop potentially previously started measurement
  error = scd4x.stopPeriodicMeasurement();
  if (error != NO_ERROR)
  {
    Serial.print("Error trying to execute stopPeriodicMeasurement(): ");
    errorToString(error, errorMessage, sizeof errorMessage);
    Serial.println(errorMessage);
  }

  // Start measurement
  error = scd4x.startPeriodicMeasurement();
  if (error != NO_ERROR)
  {
    Serial.print("Error trying to execute startPeriodicMeasurement(): ");
    errorToString(error, errorMessage, sizeof errorMessage);
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
  if (error != NO_ERROR)
  {
    Serial.print("Error trying to execute readMeasurement(): ");
    errorToString(error, errorMessage, sizeof errorMessage);
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
