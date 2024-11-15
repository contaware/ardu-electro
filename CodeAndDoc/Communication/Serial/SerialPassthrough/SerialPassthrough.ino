/*
  Serial Passthrough
  
  - Bidirectionally pass data between Serial and Serial1.
*/

void setup()
{
  Serial.begin(9600);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!
  Serial1.begin(9600);
  Serial.println("Test Serial <-> Serial1");
  Serial1.println("Test Serial <-> Serial1");
}

void loop()
{
  if (Serial.available())
    Serial1.write(Serial.read());

  if (Serial1.available())
    Serial.write(Serial1.read());
}
