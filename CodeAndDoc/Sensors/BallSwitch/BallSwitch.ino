/*
  Ball switch which opens when tilted

  - Connect one end to GND and the other to SWITCH_PIN.

  - Debounce if using as digital signal!
*/

const byte SWITCH_PIN = 8;

void setup()
{ 
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SWITCH_PIN, INPUT_PULLUP);
} 

void loop() 
{
  if (digitalRead(SWITCH_PIN) == LOW)
    digitalWrite(LED_BUILTIN, HIGH);
  else
    digitalWrite(LED_BUILTIN, LOW);
}
