/*
  Touch Sensor with TTP223 chip

  - VDD supply is 2.0V - 5.5V.
  
  - It is already debounced and returns 1 when the finger is close to the sensor
    (works also through non-metallic surfaces).
    
  - When holding down the finger some modules return 1 indefinitely (see MOTB in
    datasheet). Some other modules have a press timeout of ~6 sec after which
    they revert returning 0.
*/
const byte TOUCH_PIN = 9;

int previousSensorValue = -1;

void setup()
{
  Serial.begin(9600);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!
  pinMode(TOUCH_PIN, INPUT);
}

void loop()
{
  int sensorValue = digitalRead(TOUCH_PIN);
  
  // Toggle
  if (previousSensorValue != sensorValue)
  {
    if (previousSensorValue != -1)
    {
      if (sensorValue == 1)
        Serial.println("0 -> 1");
      else
        Serial.println("1 -> 0");
    }
    previousSensorValue = sensorValue;
  }

  // Current value
  if (sensorValue == 1)
    Serial.println("1");
  else
    Serial.println("0");

  delay(100);
}
