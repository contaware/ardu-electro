/*
  Relay Shield for ESP8266 with D1 mini form factor

  - Max load is 250VAC / 10A.

  - By default the relay pin is D1 (GPIO5).

  - Note: we can now also find ESP32 boards with 
          the D1 mini form factor.
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
