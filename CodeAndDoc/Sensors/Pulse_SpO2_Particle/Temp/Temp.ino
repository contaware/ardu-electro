/*
  The chip runs at 1.8V, for the I2C interface some modules have level 
  shifters built-in, others have pull-ups to 1.8V or 3.3V.
  
  For the modules with 1.8V pull-ups some people suggest to disconnect
  the module pull-ups and do external pull-ups to 3.3V or 5V, that
  should be ok because the maximum rating according to the datasheet for
  "All Other Pins to GND" is -0.3V to +6.0V. Adding additional 4.7K 
  pull-ups to 5V without disconnecting the existing 4.7K pull-ups also
  works (the bus high voltage is then 3.4V)

  Note: MAX30102 has IR and red LEDs
        MAX30105 has IR, red and green LEDs
*/
#include "MAX30105.h"           // SparkFun MAX3010x Pulse and Proximity Sensor Library

MAX30105 particleSensor;

void setup()
{
  Serial.begin(9600);

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) // use default I2C port, 400kHz speed
  {
    Serial.println("MAX3010X was not found. Please check wiring/power.");
    while (true);
  }

  // The LEDs are very low power and won't affect the temp reading much but
  // you may want to turn off the LEDs to avoid any local heating
  particleSensor.setup(0);

  // Enable the temp ready interrupt, this is required!
  particleSensor.enableDIETEMPRDY();
}

void loop()
{
  float temperature = particleSensor.readTemperature();
  Serial.print("Temp = ");
  Serial.print(temperature, 4);
  Serial.println(" °C");
}
