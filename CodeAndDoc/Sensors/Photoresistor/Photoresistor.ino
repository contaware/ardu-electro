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
  Serial.begin(9600);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!
}

void loop()
{
  int value = analogRead(V_PIN);
  Serial.print("Value is ");
  Serial.println(value);
  delay(1000);
}
