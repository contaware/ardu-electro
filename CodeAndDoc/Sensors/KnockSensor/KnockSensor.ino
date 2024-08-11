/*
 The knock sensor switches ON a short moment while a percussion is detected
 
 - VDD 3.3V - 5V.

 - The module has a 10K pull-up like the reed switch module.
*/

const byte KNOCK_PIN = 8;
unsigned long lastKnockMillis;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(KNOCK_PIN, INPUT);
  digitalWrite(LED_BUILTIN, LOW);
  lastKnockMillis = millis();
} 

void loop() 
{
  // Debounce and turn LED ON for 500 ms
  unsigned long currentMillis = millis();
  if (digitalRead(KNOCK_PIN) == LOW)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    lastKnockMillis = currentMillis;
  }
  else if (currentMillis - lastKnockMillis > 500U)
    digitalWrite(LED_BUILTIN, LOW);
}
