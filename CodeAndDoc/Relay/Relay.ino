/*
  When disconnecting the JDVcc jumper from Vcc we can use a separate power source for the relay coils:
  - As a normal rule, if an external power supply is used, it's important to short the Arduno ground with the external power ground.
  - But for a "total opto isolation configuration" the Arduino ground should not be connected with the external power
    ground (it works because the Arduino drives the optical coupler with an active LOW signal to the IN pin)
*/
const byte RELAY_PIN = 7;

void setup()
{
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // active LOW signal to the IN pin
}

void loop()
{

}
