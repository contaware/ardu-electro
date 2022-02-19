/*
  KY-037
  Note: - audio sensor's analog output is not amplified and thus impossible to read as analog input.
        - schematic in the Lafvin Manual is just fantasy... completly wrong!
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
