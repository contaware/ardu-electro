/*
  KY-037/KY-038 sound sensor module
 
  - VDD supply is 5V.
  
  - The only difference between KY-037 and KY-038 is the size of the MIC
    (KY-037 has a bigger MIC).
  
  - Audio sensor's analog output is not amplified and thus impossible to 
    read as analog input.
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
