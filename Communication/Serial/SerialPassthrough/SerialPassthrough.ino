/*
  Serial Passthrough
  
  - Bidirectionally pass data between Serial and Serial1.

  - Not working with boards that do not have a dedicated 
    serial port (Serial1), like Uno or Nano.
*/

void setup()
{
  // Init Serial
  Serial.begin(9600);
  Serial1.begin(9600);
}

void loop()
{
  if (Serial.available())
    Serial1.write(Serial.read());

  if (Serial1.available())
    Serial.write(Serial1.read());
}
