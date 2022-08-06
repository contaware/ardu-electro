/*
  10-axis IMU module with MPU-6050 + HMC5883L + BMP180 (or BMP085)
  
  Note: the BMP085 has been discontinued by BOSCH and replaced with the
  BMP180 which is smaller and less expensive but is identical in terms of
  wiring and software.

  - VDD supply is 3.3V - 5.5V.
  
  - The chips run at 3.3V.

  - Some modules have level shifters.

  - I2C addresses:
    BMP085/BMP180 is 0x77
    MPU6050       is 0x68
    HMC5883L      is 0x1E
*/
#include <Adafruit_BMP085.h>
#include <MPU6050.h>
#include <DFRobot_QMC5883.h>

Adafruit_BMP085 bmp;
MPU6050 accelgyro;
DFRobot_QMC5883 compass(&Wire, HMC5883L_ADDRESS); // HMC5883L_ADDRESS, QMC5883_ADDRESS, VCM5883L_ADDRESS

void setup()
{
  Serial.begin(9600);

  // Init BMP085 / BMP180
  if (!bmp.begin())
    Serial.println("Could not find the BMP085 / BMP180 sensor, check wiring!");
  else
    Serial.println("BMP085 / BMP180 found");
    
  // Init MPU6050
  accelgyro.initialize();
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
  accelgyro.setI2CBypassEnabled(true); // set bypass mode for gateway to HMC5883L otherwise it cannot be found, even with a I2C scanner

  // Init HMC5883L
  if (!compass.begin())
    Serial.println("Could not find the 5883 sensor, check wiring!");
  else
    Serial.println("5883 found");
  // Set declination angle at your location, find all declinations here:
  // https://www.magnetic-declination.com/
  // Note: declinationAngle must be set positive for EAST declinations and negative 
  //       for WEST ones. The following function inits the internal ICdeclinationAngle
  //       variable which gets added to the calculated magnetic heading when calling
  //       getHeadingDegrees(). The result is stored into the HeadingDegress variable.
  float declinationAngle = (3.0 + (11.0 / 60.0)) / (180 / PI);  // (degrees + (minutes / 60.0)) / (180 / PI)
  compass.setDeclinationAngle(declinationAngle);

  // Let it start
  delay(1000);
}

void loop()
{
  // Separator
  Serial.println("------------------");
  
  // Temperature
  Serial.print("Temp     = ");
  Serial.print(bmp.readTemperature());
  Serial.println("°C");

  // Pressure
  Serial.print("Pressure = ");
  Serial.print(bmp.readPressure());
  Serial.println("Pa");

  // Altitude
  Serial.print("Altitude = ");
  Serial.print(bmp.readAltitude(101325)); // pass pressure of the sea level at your location in Pa
  Serial.println("m");

  // Accel/gyro
  int16_t ax, ay, az;
  int16_t gx, gy, gz;
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  Serial.print("AX       = "); Serial.println(ax);
  Serial.print("AY       = "); Serial.println(ay);
  Serial.print("AZ       = "); Serial.println(az);
  
  Serial.print("GX       = "); Serial.println(gx);
  Serial.print("GY       = "); Serial.println(gy);
  Serial.print("GY       = "); Serial.println(gz);

  // Compass
  sVector_t mag = compass.readRaw();  // get the data collected by the sensor
  compass.getHeadingDegrees();        // calculates HeadingDegress from ICdeclinationAngle, YAxis and XAxis
  Serial.print("MX       = "); Serial.println(mag.XAxis);
  Serial.print("MY       = "); Serial.println(mag.YAxis);
  Serial.print("MZ       = "); Serial.println(mag.ZAxis);
  Serial.print("Heading  = "); Serial.print(mag.HeadingDegress); Serial.println("°");
  
  delay(1000);
}
