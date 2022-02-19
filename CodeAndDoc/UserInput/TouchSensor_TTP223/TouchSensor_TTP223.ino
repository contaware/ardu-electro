/*
  Grove Touch Sensor with TTP223-B chip
  - 2.0 - 5.5 V DC supply
  - Returns 1 as long as the finger is close to the sensor 
    (works also through non-metallic surfaces)
  - The nice thing is that it is already debounced!
*/
const byte TOUCH_PIN = 9;

int previousSensorValue = -1;

void setup()
{
  Serial.begin(9600);
  pinMode(TOUCH_PIN, INPUT);
}

void loop()
{
  int sensorValue = digitalRead(TOUCH_PIN);
  if (previousSensorValue != sensorValue)
  { 
    if (sensorValue == 1)
      Serial.println("1");
    else
      Serial.println("0");
    previousSensorValue = sensorValue;
  }
}
