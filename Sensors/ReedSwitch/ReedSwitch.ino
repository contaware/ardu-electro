/*
  Reed switch which short-circuits when a magnetic field is detected
 
  - VDD supply is 3.3V - 5V.

  - The module has a 10kΩ pull-up.
  
  - Debounce if using as digital signal!
*/

const byte SWITCH_PIN = 8;

void setup()
{ 
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SWITCH_PIN, INPUT);
} 

void loop() 
{
  if (digitalRead(SWITCH_PIN) == LOW)
    digitalWrite(LED_BUILTIN, HIGH);
  else
    digitalWrite(LED_BUILTIN, LOW);
}
