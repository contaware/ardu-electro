/*
  Serial Print

  Note: printing on Serial Monitor is handled a bit differently for the various platforms.
*/

void setup()
{
  Serial.begin(9600);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!
  delay(5000);      // for ESP32 and some other MCUs a delay() is needed, otherwise
                    // the messages generated in setup() can't be seen!
  Serial.println("Init");
}

void loop()
{
  static unsigned long count = 0;
  Serial.println(count++);
  delay(1000);
}
