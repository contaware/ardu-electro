/*
  HC-S501
*/
const byte PIR_PIN = 7;

int pirValue;

void setup()
{  
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop()
{
  pirValue = digitalRead(PIR_PIN);
  digitalWrite(LED_BUILTIN, pirValue);
}
