/*
  6-axis IMU module with MPU-6050 chip

  - VDD supply is 3.3V - 5.5V.
  
  - The chip runs at 3.3V.
  
  - Connecting AD0 to HIGH changes the I2C address from 0x68 to 0x69.
  
  - On the module the SDA and SCK lines have 2.2kΩ pull-ups connected to 3.3V.
  
  - The module has no level shifters, better to use external ones.

  - Gyroscope and accelerometer calibration is something we need to perform 
    regularly as the environment where the sensors are used might change.
  
  - The MPU6050 includes an embedded temperature sensor that can measure 
    temperature over the range of -40 to 85°C with accuracy of ±1°C.
    Note that this temperature measurement is of the silicon die itself 
    and not the ambient temperature. Such measurements are commonly used 
    to offset the calibration of accelerometer and gyroscope or to detect
    temperature changes rather than measuring absolute temperatures.
*/
#include <MPU6050.h>  // by ElectronicCats, https://github.com/electroniccats/mpu6050
#include <PrintCol.h>

#define MPU6050_ADDR      0x68

MPU6050 accelgyro(MPU6050_ADDR);
int16_t ax, ay, az;
int16_t gx, gy, gz;
float axOffset = 0.0;
float ayOffset = 0.0;
float azOffset = 0.0;
float axScale = 16383.5;
float ayScale = 16383.5;
float azScale = 16383.5;
float gxOffset = 0.0;
float gyOffset = 0.0;
float gzOffset = 0.0;
float gScale = 131.0;

/*
A basic gyro calibration can be performed by keeping the sensor in a 
fixed position while recording a couple of readings. The average of 
these readings can then be used as an offset value to correct the 
gyroscope output.
*/
void calibrateGyro()
{
  // Prompt
  Serial.println("---------------------------------------------");
  Serial.println("Gyroscope calibration");
  Serial.println("Keep the sensor in a fixed position");
  Serial.println("Press enter to start");
  while(Serial.read() == -1);
  
  const int16_t sampleCount = 50;
  for (int16_t i = 0; i < sampleCount; i++)
  {
    accelgyro.getRotation(&gx, &gy, &gz);
    gxOffset += gx;
    gyOffset += gy;
    gzOffset += gz; 
    delay(50);
  }
  gxOffset /= sampleCount;
  gyOffset /= sampleCount;
  gzOffset /= sampleCount;

  // Print
  Serial.print("Gyro offset ");
  printCol(gxOffset);
  printCol(gyOffset);
  printCol(gzOffset);
  Serial.println();
}

/*
For the accelerometer calibration we use gravity as our reference,
it allows us to calculate an offset and a scaling value for each axis.
To do this we have to record readings for each axis pointing towards
gravity and away from gravity. With these values we then first calculate
the offset:
offset = (max + min) / 2

And as we know that when the axis is aligned with gravity it should 
provide a reading of 1g we can calculate a scaling factor:
scale = (max – offset) = (max - min) / 2

There is also a faster calibration method which only requires the 
module being still and in its up position, see here:
https://github.com/rfetick/MPU6050_light
https://github.com/wollewald/MPU9250_WE
*/
void calibrateAccel()
{
  float minX, maxX, minY, maxY, minZ, maxZ;

  // Prompt
  Serial.println("---------------------------------------------");
  Serial.println("Accelerometer calibration");
  Serial.println("Place sensor Z-axis up and press enter");
  while(Serial.read() == -1);
  accelgyro.getAcceleration(&ax, &ay, &az);
  maxZ = az;

  Serial.println("Place sensor Z-axis down and press enter");
  while(Serial.read() == -1);
  accelgyro.getAcceleration(&ax, &ay, &az);
  minZ = az;

  Serial.println("Place sensor Y-axis up and press enter");
  while(Serial.read() == -1);
  accelgyro.getAcceleration(&ax, &ay, &az);
  maxY = ay;

  Serial.println("Place sensor Y-axis down and press enter");
  while(Serial.read() == -1);
  accelgyro.getAcceleration(&ax, &ay, &az);
  minY = ay;

  Serial.println("Place sensor X-axis up and press enter");
  while(Serial.read() == -1);
  accelgyro.getAcceleration(&ax, &ay, &az);
  maxX = ax;

  Serial.println("Place sensor X-axis down and press enter");
  while(Serial.read() == -1);
  accelgyro.getAcceleration(&ax, &ay, &az);
  minX = ax;

  // Calculate X-axis offset and scale
  axOffset = (minX + maxX) / 2.0;
  axScale = maxX - axOffset;

  // Calculate Y-axis offset and scale
  ayOffset = (minY + maxY) / 2.0;
  ayScale = maxY - ayOffset;

  // Calculate Z-axis offset and scale
  azOffset = (minZ + maxZ) / 2.0;
  azScale = maxZ - azOffset;

  // Print
  Serial.print("Accel offset");
  printCol(axOffset);
  printCol(ayOffset);
  printCol(azOffset);
  Serial.println();
  Serial.print("Accel scale ");
  printCol(axScale);
  printCol(ayScale);
  printCol(azScale);
  Serial.println();
}

void setup()
{
  Serial.begin(9600);
    
  // Init MPU6050
  accelgyro.initialize(); // this sets the accelerometer and the gyroscope to their most sensitive settings,
                          // namely +/- 2g and +/- 250 degrees/sec. Note: it would even not be necessary to 
                          // set those two values because the chip starts with the registers set to 0 which 
                          // corresponds to +/- 2g and +/- 250 degrees/sec.
  
  // Set ranges if they differ from the initialize() default
  accelgyro.setFullScaleAccelRange(MPU6050_ACCEL_FS_2); // MPU6050_ACCEL_FS_2, MPU6050_ACCEL_FS_4, MPU6050_ACCEL_FS_8, MPU6050_ACCEL_FS_16
  uint8_t gyroRange = MPU6050_GYRO_FS_250;              // MPU6050_GYRO_FS_250, MPU6050_GYRO_FS_500, MPU6050_GYRO_FS_1000, MPU6050_GYRO_FS_2000
  accelgyro.setFullScaleGyroRange(gyroRange);
  switch (gyroRange)
  {
    case MPU6050_GYRO_FS_250:   gScale = 131;   break;  // 32767 / 250
    case MPU6050_GYRO_FS_500:   gScale = 65.5;  break;  // 32767 / 500
    case MPU6050_GYRO_FS_1000:  gScale = 32.8;  break;  // 32767 / 1000
    case MPU6050_GYRO_FS_2000:  gScale = 16.4;  break;  // 32767 / 2000
    default: break;
  }
    
  // Check
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

  // Calibrate
  calibrateGyro();
  calibrateAccel();

  // Temperature
  Serial.println("---------------------------------------------");
  Serial.print("Chip temp °C"); 
  printCol((accelgyro.getTemperature() / 340.0) + 36.53); // equation for temperature from datasheet
  Serial.println();

  // Header
  Serial.println("---------------------------------------------");
  Serial.println("                      X          Y          Z");
}
  
void loop()
{
  Serial.println("---------------------------------------------");

  // Accelerometer and Gyroscope
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  Serial.print("Accel g     ");
  printCol((ax - axOffset) / axScale);
  printCol((ay - ayOffset) / ayScale);
  printCol((az - azOffset) / azScale);
  Serial.println();  
  Serial.print("Gyro °/s    ");
  printCol((gx - gxOffset) / gScale);
  printCol((gy - gyOffset) / gScale);
  printCol((gz - gzOffset) / gScale);
  Serial.println();

  // Note: - convert the acceleration from g unit to m/s^2 multiplying by 9.81
  //       - convert the velocity from degrees/sec to rad/s multiplying by 2*PI/360 = 0.017453293
  
  delay(500);
}
