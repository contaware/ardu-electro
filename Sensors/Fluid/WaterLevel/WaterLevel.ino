/*
  Water Level Detection Sensor

  - VDD supply is 3.3V - 5V.

  - The sensor traces resistance varies inversely with the depth of 
    immersion in water. The common collector circuit generates an output 
    voltage proportional to the current flowing into the transistor 
    base, thus when the sensor resistance lowers, the output voltage 
    increases.
*/
const byte SIGNAL_PIN = A0;

void setup()
{
  // Init Serial
  Serial.begin(9600);
}

void loop()
{
  int value = analogRead(SIGNAL_PIN);
  Serial.print("Water Level is = ");
  Serial.println(value);
  delay(1000);
}
