/*
 In this code with use the Arduino 4 Relays Shield

 - Max load is 48V / 60W.
 
 - Coil current consumption is 35mA for each relay.
 
 - Note that there are many other shields or modules, for example some of the modules
   have a JDVcc jumper which permits the use of a separate power source for the relay
   coils, and some have also a "total opto isolation configuration" which allows us 
   to not connect the Arduino ground with the external power ground (it works because
   the Arduino drives the optical coupler with an active LOW signal to the IN pin). 
*/
const byte RELAY1_PIN = 4;
const byte RELAY2_PIN = 7;
const byte RELAY3_PIN = 8;
const byte RELAY4_PIN = 12;

void setup()
{
  // Pins default to inputs, set them LOW before switching to outputs
  digitalWrite(RELAY1_PIN, LOW);
  digitalWrite(RELAY2_PIN, LOW);
  digitalWrite(RELAY3_PIN, LOW);
  digitalWrite(RELAY4_PIN, LOW);
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);
  pinMode(RELAY4_PIN, OUTPUT);

  delay(5000);  

  // All relays ON
  digitalWrite(RELAY1_PIN, HIGH);
  digitalWrite(RELAY2_PIN, HIGH);
  digitalWrite(RELAY3_PIN, HIGH);
  digitalWrite(RELAY4_PIN, HIGH);
}

void loop()
{

}
