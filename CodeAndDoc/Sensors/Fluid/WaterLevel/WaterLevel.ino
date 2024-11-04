/*
  Water Level Detection Sensor
*/
const byte SIGNAL_PIN = A0;

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  int value = analogRead(SIGNAL_PIN);
  Serial.print("Water Level is = ");
  Serial.println(value);
  delay(1000);
}
