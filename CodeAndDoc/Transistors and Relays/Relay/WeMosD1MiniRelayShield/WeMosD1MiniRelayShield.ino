/*
 In this code with use the Wemos D1 Mini Relay Shield

 - Max load is 250VAC / 10A.
 
 - By default the relay pin is D1 (GPIO5)
*/
const int relayPin = D1;

void setup()
{
  // Pins default to inputs, set them LOW before switching to outputs
  digitalWrite(relayPin, LOW);
  pinMode(relayPin, OUTPUT);
  
  delay(5000);
  
  digitalWrite(relayPin, HIGH); // turns relay ON
}

void loop()
{

}
