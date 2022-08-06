/*
  Digital Compass / Magnetometer HMC5883L, QMC5883L or VCM5883L

  - VDD supply is 3.3V - 5V.
  
  - The chip runs at 3.3V.
  
  - Some modules have level shifters.
  
  - Honeywell discontinued the HMC5883L and HMC5983 (same as HMC5883L but with temperature compensation)
    in 2016, but licensed the technology to QST Corporation, who now manufactures the replacement QMC5883L.
    While the QMC5883L is pin to pin compatible, its registers are different.
    * If on the chip it's written L883 then it's the original Honeywell HMC5883L with I2C address 0x1E.
    * If on the chip it's written DB5883 then it's the QMC5883L with I2C address 0x0D.
    * There is also a VCM5883L chip with I2C address 0x0C.
   
  - The four cardinal points of a compass are North 0° (or 360°)
                                              East  90°
                                              South 180°
                                              West  270°
*/
#include <DFRobot_QMC5883.h>

DFRobot_QMC5883 compass(&Wire, HMC5883L_ADDRESS); // HMC5883L_ADDRESS, QMC5883_ADDRESS, VCM5883L_ADDRESS 

void setup()
{
  Serial.begin(9600);
  while (!compass.begin())
  {
    Serial.println("Could not find the 5883 sensor, check wiring or try another address!");
    delay(1000);
  }

  // Set declination angle at your location, find all declinations here:
  // https://www.magnetic-declination.com/
  // Note: declinationAngle must be set positive for EAST declinations and negative 
  //       for WEST ones. The following function inits the internal ICdeclinationAngle
  //       variable which gets added to the calculated magnetic heading when calling
  //       getHeadingDegrees(). The result is stored into the HeadingDegress variable.
  float declinationAngle = (3.0 + (11.0 / 60.0)) / (180 / PI);  // (degrees + (minutes / 60.0)) / (180 / PI)
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
  delay(1000);
}

void loop()
{
  sVector_t mag = compass.readRaw();  // get the data collected by the sensor
  compass.getHeadingDegrees();        // calculates HeadingDegress from ICdeclinationAngle, YAxis and XAxis
  Serial.println("---------------------------------");
  Serial.print("X=");
  Serial.print(mag.XAxis);
  Serial.print(" Y=");
  Serial.print(mag.YAxis);
  Serial.print(" Z=");
  Serial.print(mag.ZAxis);
  Serial.print(" Degrees=");
  Serial.println(mag.HeadingDegress);
  delay(700);
}
