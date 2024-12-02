/*
  Digital Compass / Magnetometer HMC5883L, QMC5883L or VCM5883L

  - VDD supply is 3.3V - 5V.
  
  - The chip runs at 3.3V.
  
  - Some modules have level shifters.

  - The compass outputs 16-bit 2's complement numbers with a range from 
    -2048 to 2047 given by the 12-bit ADC.
    Note: if the ADC reading overflows or underflows, or if there is a 
          math overflow during the bias measurement, then the returned 
          value is -4096. If a sensor is defective it can also return 
          -4096.
  
  - Honeywell discontinued the HMC5883L and HMC5983 (same as HMC5883L 
    but with temperature compensation) in 2016, but licensed the 
    technology to QST Corporation, who now manufactures the replacement 
    QMC5883L. While the QMC5883L is pin to pin compatible, its registers 
    are different.
    * If on the chip it's written L883 then it's the original Honeywell 
      HMC5883L with I2C address 0x1E.
    * If on the chip it's written DB5883 or HA5883 then it's the 
      QMC5883L with I2C address 0x0D.
    * There is also a VCM5883L chip with I2C address 0x0C.

  - The purpose of calibration is to transform displaced eliptical 
    shapes into nice balanced spheres centered on the origin. Hard iron 
    shifts the entire sensor output, while soft iron scales it. 
    Calibration must be repeated from time to time to avoid performance 
    degradation due to component drift and aging.
    https://www.appelsiini.net/2018/calibrate-magnetometer/
    https://github.com/mprograms/QMC5883LCompass/blob/master/src/QMC5883LCompass.cpp
    https://github.com/kriswiner/MPU6050/wiki/Simple-and-Effective-Magnetometer-Calibration
    https://diydrones.com/profiles/blogs/advanced-hard-and-soft-iron-magnetometer-calibration-for-dummies   
*/
#include <DFRobot_QMC5883.h>
#include <PrintCol.h>

// Set declination angle at your location, find all declinations here:
// https://www.magnetic-declination.com/
// Note: declinationAngle must be set positive for EAST declinations and 
//       negative for WEST ones. The library's setDeclinationAngle() 
//       inits the internal ICdeclinationAngle variable which gets added 
//       to the calculated magnetic heading when calling getHeadingDegrees(). 
//       The result is stored into the HeadingDegress variable. 
//       Unfortunatelly the library allows the access of HeadingDegress 
//       only through readRaw(), but that reads again from the 
//       magnetometer. Until it's fixed we calculate it with our 
//       calcHeadingDegrees() function.
const float declinationAngle = (3.0 + (11.0 / 60.0)) / (180 / PI);  // (degrees + (minutes / 60.0)) / (180 / PI)
DFRobot_QMC5883 compass(&Wire, HMC5883L_ADDRESS); // HMC5883L_ADDRESS, QMC5883_ADDRESS, VCM5883L_ADDRESS 
int minX = 32767;
int maxX = -32768;
int minY = 32767;
int maxY = -32768;
int minZ = 32767;
int maxZ = -32768;
int offsetX = 0;
int offsetY = 0;
int offsetZ = 0;
int deltaX = 0;
int deltaY = 0;
int deltaZ = 0;
float scaleX = 1.0;
float scaleY = 1.0;
float scaleZ = 1.0;

void setup()
{
  // Init Serial (leave Serial Monitor open to see all messages)
  Serial.begin(9600); delay(5000); // wait 5s that Serial is ready
  Serial.println("5883 Test");

  // Init compass
  if (!compass.begin())
  {
    Serial.println("Could not find the 5883 sensor, check wiring or try another address!");
    while (true);
  }
  compass.setDeclinationAngle(declinationAngle);
  
  if (compass.isHMC())
  {
    Serial.println("Initialize HMC5883");

    // Set/get the compass signal gain range
    compass.setRange(HMC5883L_RANGE_1_3GA);         // default: HMC5883L_RANGE_1_3GA
    Serial.print("compass range is: ");
    Serial.println(compass.getRange());

    // Set/get measurement mode
    compass.setMeasurementMode(HMC5883L_CONTINOUS); // default: HMC5883L_CONTINOUS
    Serial.print("compass measurement mode is: ");
    Serial.println(compass.getMeasurementMode());

    // Set/get the data collection frequency of the sensor
    compass.setDataRate(HMC5883L_DATARATE_15HZ);    // default: HMC5883L_DATARATE_15HZ
    Serial.print("compass data rate is: ");
    Serial.println(compass.getDataRate());

    // Set/get sensor samples
    compass.setSamples(HMC5883L_SAMPLES_8);        // default: HMC5883L_SAMPLES_1
    Serial.print("compass samples is: ");
    Serial.println(compass.getSamples());
  }
  else if (compass.isQMC())
  {
    Serial.println("Initialize QMC5883");

    // Set/get the compass signal gain range
    compass.setRange(QMC5883_RANGE_2GA);            // default: QMC5883_RANGE_8GA
    Serial.print("compass range is: ");
    Serial.println(compass.getRange());

    // Set/get measurement mode
    compass.setMeasurementMode(QMC5883_CONTINOUS);  // default: QMC5883_CONTINOUS
    Serial.print("compass measurement mode is: ");
    Serial.println(compass.getMeasurementMode());

    // Set/get the data collection frequency of the sensor
    compass.setDataRate(QMC5883_DATARATE_50HZ);     // default: QMC5883_DATARATE_50HZ
    Serial.print("compass data rate is: ");
    Serial.println(compass.getDataRate());

    // Set/get sensor samples
    compass.setSamples(QMC5883_SAMPLES_8);          // default: QMC5883_SAMPLES_8
    Serial.print("compass samples is: ");
    Serial.println(compass.getSamples());
  }
  else if (compass.isVCM())
  {
    Serial.println("Initialize VCM5883L");

    // Set/get measurement mode
    compass.setMeasurementMode(VCM5883L_CONTINOUS);
    Serial.print("compass measurement mode is: ");
    Serial.println(compass.getMeasurementMode());

    // Set/get the data collection frequency of the sensor
    compass.setDataRate(VCM5883L_DATARATE_50HZ);
    Serial.print("compass data rate is: ");
    Serial.println(compass.getDataRate());
  }
  
  // Calibration prompt
  Serial.println("-------------------------------------------------------------");
  Serial.println("Magnetometer calibration");
  Serial.println("Please rotate the sensor along each axis a few times");
  Serial.println("Press enter to start");
  while(Serial.read() == -1);

  const int16_t sampleCount = 100;
  for (int16_t i = 0 ; i < sampleCount ; i++)
  {
    measureMinMax();
    delay(200);
  }
  offsetX = (maxX + minX) / 2;
  offsetY = (maxY + minY) / 2;
  offsetZ = (maxZ + minZ) / 2;
  deltaX = (maxX - minX) / 2;
  deltaY = (maxY - minY) / 2;
  deltaZ = (maxZ - minZ) / 2;
  int avgDelta = (deltaX + deltaY + deltaZ) / 3;
  if (deltaX != 0)
    scaleX = (float)avgDelta / deltaX;
  if (deltaY != 0)
    scaleY = (float)avgDelta / deltaY;
  if (deltaZ != 0)
    scaleZ = (float)avgDelta / deltaZ;

  // Print calibration results
  Serial.print("Mag offset       ");
  printCol(offsetX);
  printCol(offsetY);
  printCol(offsetZ);
  Serial.println();
  Serial.print("Mag scale        ");
  printCol(scaleX);
  printCol(scaleY);
  printCol(scaleZ);
  Serial.println();
  
  // Header
  Serial.println("-------------------------------------------------------------");
  Serial.println("         (Heading: North 0°  East 90°  South 180°  West 270°)");
  Serial.println("                          X          Y          Z   Heading °");
}

void measureMinMax()
{
  sVector_t mag = compass.readRaw(); // get the data collected by the sensor

  if (mag.XAxis < minX)
    minX = mag.XAxis;
  if (mag.XAxis > maxX)
    maxX = mag.XAxis;
  if (mag.YAxis < minY)
    minY = mag.YAxis;
  if (mag.YAxis > maxY) 
    maxY = mag.YAxis;
  if (mag.ZAxis < minZ)
    minZ = mag.ZAxis;
  if (mag.ZAxis > maxZ) 
    maxZ = mag.ZAxis;

  Serial.print("Mag calib...     ");
  printCol(mag.XAxis);
  printCol(mag.YAxis);
  printCol(mag.ZAxis);
  Serial.println();
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
  Serial.println("-------------------------------------------------------------");
  
  // Uncorrected
  sVector_t magRaw = compass.readRaw(); // get the data collected by the sensor
  calcHeadingDegrees(magRaw);

  // Correct offset (hard iron distortion)
  sVector_t magOffsetFix;
  magOffsetFix.XAxis = magRaw.XAxis - offsetX;
  magOffsetFix.YAxis = magRaw.YAxis - offsetY;
  magOffsetFix.ZAxis = magRaw.ZAxis - offsetZ;
  calcHeadingDegrees(magOffsetFix);

  // Correct offset + scale; usually that's not necessary because
  // there is far less soft iron distortion than hard iron distortion
  sVector_t magFullFix;
  magFullFix.XAxis = (magRaw.XAxis - offsetX) * scaleX;
  magFullFix.YAxis = (magRaw.YAxis - offsetY) * scaleY;
  magFullFix.ZAxis = (magRaw.ZAxis - offsetZ) * scaleZ;
  calcHeadingDegrees(magFullFix);

  // Print
  Serial.print("Mag raw          ");
  printCol(magRaw.XAxis);
  printCol(magRaw.YAxis);
  printCol(magRaw.ZAxis);
  printCol(magRaw.HeadingDegress);
  Serial.println();
  Serial.print("Mag offset calib.");
  printCol(magOffsetFix.XAxis);
  printCol(magOffsetFix.YAxis);
  printCol(magOffsetFix.ZAxis);
  printCol(magOffsetFix.HeadingDegress);
  Serial.println();
  Serial.print("Mag full calib.  ");
  printCol(magFullFix.XAxis);
  printCol(magFullFix.YAxis);
  printCol(magFullFix.ZAxis);
  printCol(magFullFix.HeadingDegress);
  Serial.println();

  delay(700);
}
