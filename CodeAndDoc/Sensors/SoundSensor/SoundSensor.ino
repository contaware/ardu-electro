/*
  KY-037 sound sensor module
 
  - VDD supply is 5V.
  - Audio sensor's analog output is not amplified and thus impossible to read as analog input.
  - Schematic in the Lafvin Manual is just fantasy... completly wrong!
*/

const byte AUDIO_SENSOR_PIN = 3;

void setup()
{
  pinMode(AUDIO_SENSOR_PIN, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  if (digitalRead(AUDIO_SENSOR_PIN))
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
  }
}
