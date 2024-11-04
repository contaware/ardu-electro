void setup()
{
  Serial.begin(9600);
}

void loop()
{
  int value = analogRead(A0);
  Serial.print("Value is = ");
  Serial.println(value);
  delay(1000);
}
