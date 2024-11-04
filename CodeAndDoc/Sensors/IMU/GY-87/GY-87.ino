/*
  10-axis IMU module with MPU-6050 + HMC5883L (or QMC5883L) + BMP180 (or BMP085)

  - VDD supply is 3.3V - 5.5V.
  
  - The chips run at 3.3V.

  - The module has level shifters.

  - The MPU6050 is configured to use the default I2C address 0x68.

  - Honeywell discontinued the HMC5883L and HMC5983 (same as HMC5883L but with temperature compensation)
    in 2016, but licensed the technology to QST Corporation, who now manufactures the replacement QMC5883L.
    While the QMC5883L is pin to pin compatible, its registers are different.
    * If on the chip it's written L883 then it's the original Honeywell HMC5883L with I2C address 0x1E.
    * If on the chip it's written DB5883 or HA5883 then it's the QMC5883L with I2C address 0x0D.
     
  - The BMP085 has been discontinued by BOSCH and replaced with the BMP180 which is smaller and less 
    expensive but is identical in terms of wiring and software. Both use the I2C address 0x77.
*/
#include <Adafruit_BMP085.h>
#include <MPU6050.h>
#include <DFRobot_QMC5883.h>
#include <PrintCol.h>

Adafruit_BMP085 bmp;
MPU6050 accelgyro;
// Set declination angle at your location, find all declinations here:
// https://www.magnetic-declination.com/
// Note: declinationAngle must be set positive for EAST declinations and negative 
//       for WEST ones. The library's setDeclinationAngle() inits the internal 
//       ICdeclinationAngle variable which gets added to the calculated magnetic 
//       heading when calling getHeadingDegrees(). The result is stored into the 
//       HeadingDegress variable. Unfortunatelly the library allows the access of 
//       HeadingDegress only through readRaw(), but that reads again from the magnetometer.
//       Until it's fixed we calculate it with our calcHeadingDegrees() function.
const float declinationAngle = (3.0 + (11.0 / 60.0)) / (180 / PI);  // (degrees + (minutes / 60.0)) / (180 / PI)
DFRobot_QMC5883 compass(&Wire, QMC5883_ADDRESS); // HMC5883L_ADDRESS, QMC5883_ADDRESS, VCM5883L_ADDRESS

void setup()
{
  Serial.begin(9600);

  // Init BMP085 / BMP180
  // Note: that must happen first, before the MPU-6050 initialization, 
  //       otherwise it's not working, I do not know why...
  if (!bmp.begin())
    Serial.println("Could not find the BMP085 / BMP180 sensor, check wiring!");
  else
    Serial.println("BMP085 / BMP180 found");
    
  // Init MPU6050
  accelgyro.initialize();
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
  accelgyro.setI2CBypassEnabled(true); // set bypass mode for gateway to 5883 otherwise it cannot be found, even with a I2C scanner
  
  // Init 5883
  if (!compass.begin())
    Serial.println("Could not find the 5883 sensor, check wiring or try another address!");
  else
    Serial.println("5883 found");
  compass.setDeclinationAngle(declinationAngle);

  // Let it start
  delay(1000);
}

void calcHeadingDegrees(sVector_t& v)
{
  float heading = atan2(v.YAxis ,v.XAxis);
  heading += declinationAngle;
  if (heading < 0)
    heading += 2*PI;
  if (heading > 2*PI)
    heading -= 2*PI;
  v.HeadingDegress = heading * 180/PI;
}

void loop()
{
  Serial.println("----------------------------------------------------------");

  // Environment
  Serial.println("               MPU6050 °C     Air °C  Pres. hPa     Alt. m");
  Serial.print("Environment   ");
  printCol(bmp.readTemperature());
  printCol((accelgyro.getTemperature() / 340.0) + 36.53); // equation for temperature from datasheet
  printCol(bmp.readPressure() / 100.0);
  printCol(bmp.readAltitude(101325)); // pass the pressure of the sea level at your location in Pa
  Serial.println(); Serial.println();

  // Accel/gyro/compass
  Serial.println("                        X          Y          Z  Heading °");
  int16_t ax, ay, az, gx, gy, gz;
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  sVector_t mag = compass.readRaw();  // get the data collected by the sensor
  calcHeadingDegrees(mag);
  Serial.print("Accelerometer "); printCol(ax);        printCol(ay);        printCol(az); Serial.println();
  Serial.print("Gyroscope     "); printCol(gx);        printCol(gy);        printCol(gz); Serial.println();
  Serial.print("Magnetometer  "); printCol(mag.XAxis); printCol(mag.YAxis); printCol(mag.ZAxis);
  printCol(mag.HeadingDegress); Serial.println("");
  
  delay(1000);
}
