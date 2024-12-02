/*
  MAX471 high-side current measurement through an internal shunt

  - The MAX471 takes the power required for itself from the circuit to 
    be measured, which must be in the range of 3V - 36V. This means that
    RS+/RS- must be connected to the high side of the load.
  
  - It measures up to 3A (MAX471 power consumption < 100μA).
  
  - The shunt is of 35mΩ, with a 2kΩ resistor from OUT to GND the 
    voltage seen at OUT is 1V/A; this voltage is always positive 
    regardless of the current direction. The open-collector SIGN output 
    indicates the current flow direction.
*/
const byte OUT_PIN = A0;
const byte SIGN_PIN = 8;

void setup()
{
  // Init Serial
  Serial.begin(9600);

  // Set pin mode
  pinMode(SIGN_PIN, INPUT_PULLUP);
}

void loop()
{
  int sign = digitalRead(SIGN_PIN);
  int out = analogRead(OUT_PIN);
  float current = out * 5.0 / 1.023;
  Serial.write(sign ? '+' : '-');
  Serial.print(current);
  Serial.println("mA");
  delay(1000);       
}
