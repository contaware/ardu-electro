/*
  Test a photoresistor
  
  - With no light hitting the photoresistor, the resistance is high.
    When light hits the photoresistor, the resistance decreases.
    
    VCC
     |
    .-.
    | | R
    | | Photoresistor
    '-'
     |
     o---------o A0
     |
    .-.
    | | R1
    | | 4.7kΩ
    '-'
     |
    GND
*/
const byte V_PIN = A0; // analog pin to read V

void setup()
{
  // Init Serial
  Serial.begin(9600);
}

void loop()
{
  int value = analogRead(V_PIN);
  Serial.print("Value is ");
  Serial.println(value);
  delay(1000);
}
