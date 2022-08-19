/*
  6-axis IMU module with MPU-6500 chip 

  - Compared to the older 6050, the 6500 has a faster update rate, a wider
    signal bandwidth, is smaller and consumes less. The only drawback is 
    that the 6500 outputs are a bit more noisy. A great improvement is the
    6500 registers documentation, the 6050 is really bad in this regard.
  
  - VDD supply is 3.3V - 5.5V.
  
  - The chip runs at 3.3V.
  
  - Connecting AD0 to HIGH changes the I2C address from 0x68 to 0x69.
  
  - On the module the SDA and SCK lines have 10k pull-ups connected to 3.3V.
  
  - The module has no level shifters, better to use external ones.

  - Gyroscope and accelerometer calibration is something we need to perform 
    regularly as the environment where the sensors are used might change.
  
  - The MPU6500 includes an embedded temperature sensor, but this temperature
    measurement is of the silicon die itself and not the ambient temperature.
    Such measurements are commonly used to offset the calibration of 
    accelerometer and gyroscope or to detect temperature changes rather than 
    measuring absolute temperatures.

  - Attention: MPU-6500 is an MPU-9250 without a magnetometer (some modules 
    are sold as MPU-9250 but in reality they are MPU-6500). The MPU-9250 has 
    a WHO_AM_I_CODE of 0x71 (113), while the MPU-6500 0x70 (112).
*/
#include <MPU6500_WE.h> // https://wolles-elektronikkiste.de/en/mpu9250-9-axis-sensor-module-part-1
#include <PrintCol.h>

#define MPU6500_ADDR      0x68

/* There are several ways to create your MPU6500 object:
 * MPU6500_WE myMPU6500 = MPU6500_WE()              -> uses Wire / I2C Address = 0x68
 * MPU6500_WE myMPU6500 = MPU6500_WE(MPU6500_ADDR)  -> uses Wire / MPU6500_ADDR
 * MPU6500_WE myMPU6500 = MPU6500_WE(&wire2)        -> uses the TwoWire object wire2 / MPU6500_ADDR
 * MPU6500_WE myMPU6500 = MPU6500_WE(&wire2, MPU6500_ADDR) -> all together
 */
MPU6500_WE myMPU6500 = MPU6500_WE(MPU6500_ADDR);

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  
  delay(1000);

  // Init MPU6500
  if (!myMPU6500.init())
    Serial.println("MPU6500 does not respond");
  else
    Serial.println("MPU6500 is connected");

  /* The slope of the curve of acceleration vs measured values fits quite well to the theoretical 
   * values, e.g. 16384 units/g in the +/- 2g range. But the starting point, if you position the 
   * MPU6500 flat, is not necessarily 0g/0g/1g for x/y/z. The autoOffset function measures offset 
   * values. It assumes your MPU6500 is positioned flat with its x,y-plane. The more you deviate 
   * from this, the less accurate will be your results.
   * The function also measures the offset of the gyroscope data. The gyroscope offset does not   
   * depend on the positioning.
   * This function needs to be called at the beginning since it can overwrite your settings!
   */
  Serial.println("--------------------------------------------------------");
  Serial.println("Position your MPU6500 flat and don't move it");
  Serial.println("Calibrating...");
  delay(2000);
  myMPU6500.autoOffsets();
  Serial.println("Done!");
  
  /*  This is a more accurate method for calibration. You have to determine the minimum and maximum 
   *  raw acceleration values of the axes determined in the range +/- 2 g. 
   *  You call the function as follows: setAccOffsets(xMin,xMax,yMin,yMax,zMin,zMax);
   *  Use either autoOffset or setAccOffsets, not both.
   */
  //myMPU6500.setAccOffsets(-14240.0, 18220.0, -17280.0, 15590.0, -20930.0, 12080.0);

  /*  The gyroscope data is not zero, even if you don't move the MPU6500. 
   *  To start at zero, you can apply offset values. These are the gyroscope raw values you obtain
   *  using the +/- 250 degrees/s range. 
   *  Use either autoOffset or setGyrOffsets, not both.
   */
  //myMPU6500.setGyrOffsets(45.0, 145.0, -105.0);

  /*  You can enable or disable the digital low pass filter (DLPF). If you disable the DLPF, you 
   *  need to select the bandwdith, which can be either 8800 or 3600 Hz. 8800 Hz has a shorter delay,
   *  but higher noise level. If DLPF is disabled, the output rate is 32 kHz.
   *  MPU6500_BW_WO_DLPF_3600 
   *  MPU6500_BW_WO_DLPF_8800
   */
  myMPU6500.enableGyrDLPF();
  //myMPU6500.disableGyrDLPF(MPU6500_BW_WO_DLPF_8800); // bandwdith without DLPF
  
  /*  Digital Low Pass Filter for the gyroscope must be enabled to choose the level. 
   *  MPU6500_DPLF_0, MPU6500_DPLF_2, ...... MPU6500_DPLF_7 
   *  
   *  DLPF    Bandwidth [Hz]   Delay [ms]   Output Rate [kHz]
   *    0         250            0.97             8
   *    1         184            2.9              1
   *    2          92            3.9              1
   *    3          41            5.9              1
   *    4          20            9.9              1
   *    5          10           17.85             1
   *    6           5           33.48             1
   *    7        3600            0.17             8
   *    
   *    You achieve lowest noise using level 6  
   */
  myMPU6500.setGyrDLPF(MPU6500_DLPF_6);

  /*  Sample rate divider divides the output rate of the gyroscope and accelerometer.
   *  Sample rate = Internal sample rate / (1 + divider) 
   *  It can only be applied if the corresponding DLPF is enabled and 0<DLPF<7!
   *  Divider is a number 0...255
   */
  myMPU6500.setSampleRateDivider(5);

  /*  MPU6500_GYRO_RANGE_250       250 degrees per second (default)
   *  MPU6500_GYRO_RANGE_500       500 degrees per second
   *  MPU6500_GYRO_RANGE_1000     1000 degrees per second
   *  MPU6500_GYRO_RANGE_2000     2000 degrees per second
   */
  myMPU6500.setGyrRange(MPU6500_GYRO_RANGE_250);

  /*  MPU6500_ACC_RANGE_2G      2 g   (default)
   *  MPU6500_ACC_RANGE_4G      4 g
   *  MPU6500_ACC_RANGE_8G      8 g   
   *  MPU6500_ACC_RANGE_16G    16 g
   */
  myMPU6500.setAccRange(MPU6500_ACC_RANGE_2G);

  /*  Enable/disable the digital low pass filter for the accelerometer 
   *  If disabled the bandwidth is 1.13 kHz, delay is 0.75 ms, output rate is 4 kHz
   */
  myMPU6500.enableAccDLPF(true);

  /*  Digital low pass filter (DLPF) for the accelerometer, if enabled 
   *  MPU6500_DPLF_0, MPU6500_DPLF_2, ...... MPU6500_DPLF_7 
   *   DLPF     Bandwidth [Hz]      Delay [ms]    Output rate [kHz]
   *     0           460               1.94           1
   *     1           184               5.80           1
   *     2            92               7.80           1
   *     3            41              11.80           1
   *     4            20              19.80           1
   *     5            10              35.70           1
   *     6             5              66.96           1
   *     7           460               1.94           1
   */
  myMPU6500.setAccDLPF(MPU6500_DLPF_6);

  /* You can enable or disable the axes for gyroscope and/or accelerometer measurements.
   * By default all axes are enabled. Parameters are:  
   * MPU6500_ENABLE_XYZ  //all axes are enabled (default)
   * MPU6500_ENABLE_XY0  // X, Y enabled, Z disabled
   * MPU6500_ENABLE_X0Z   
   * MPU6500_ENABLE_X00
   * MPU6500_ENABLE_0YZ
   * MPU6500_ENABLE_0Y0
   * MPU6500_ENABLE_00Z
   * MPU6500_ENABLE_000  // all axes disabled
   */
  //myMPU6500.enableAccAxes(MPU6500_ENABLE_XYZ);
  //myMPU6500.enableGyrAxes(MPU6500_ENABLE_XYZ);

  // Temperature
  Serial.println("--------------------------------------------------------");
  Serial.print("Chip temp °C"); 
  printCol(myMPU6500.getTemperature());
  Serial.println();

  // Header
  Serial.println("--------------------------------------------------------");
  Serial.println("                      X          Y          Z      Res g");
}

void loop()
{
  Serial.println("--------------------------------------------------------");

  // Accelerometer and Gyroscope
  xyzFloat a = myMPU6500.getGValues();
  xyzFloat g = myMPU6500.getGyrValues();
  float resultantG = myMPU6500.getResultantG(a); // = sqrt((a.x)^2+(a.y)^2+(a.z)^2) = 1 (if module is not moving)
  Serial.print("Accel g     ");
  printCol(a.x);
  printCol(a.y);
  printCol(a.z);
  printCol(resultantG);
  Serial.println();
  Serial.print("Gyro °/s    ");
  printCol(g.x);
  printCol(g.y);
  printCol(g.z);
  Serial.println();

  delay(500);
}
