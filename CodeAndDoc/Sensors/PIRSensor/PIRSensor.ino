/*
  HC-SR501 PIR sensor module

  - VDD supply is 4.5V - 20V.
  
  - HIGH level on output pin is 3.3V.
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
