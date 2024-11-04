const byte BUZZER_PIN = 12;

void setup()
{
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop()
{
  // Output a frequency
  for (int i = 0 ; i < 80 ; i++)
  {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(1); // wait for 1ms
    digitalWrite(BUZZER_PIN, LOW);
    delay(1); // wait for 1ms
  }

  // Output another frequency
  for (int i = 0 ; i < 100 ; i++)
  {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(2); // wait for 2ms
    digitalWrite(BUZZER_PIN, LOW);
    delay(2); // wait for 2ms
  }
} 
