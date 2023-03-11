/*
  INA169 high-side current measurement through an external shunt

  - INA169 is a unipolar device, if a more negative voltage is applied
    to VIN+ with respect to VIN-, the output is zero, but it will not
    cause damage.

  - Independent power supply in the range of 2.7V - 60V; remember to
    to connect the load ground with the power ground!
  
  - Vout range depends on the voltage supplied by Vcc and the input 
    voltages can exceed the power supply voltage but must be in the
    2.7V - 60V range.

  - With the module's 0.1Ω shunt resistor we measure 1V/A.
*/
const byte OUT_PIN = A0;

void setup()
{
  Serial.begin(9600);
  Serial.println("INA169 Test");
  Serial.println();
}

void loop()
{
  int out = analogRead(OUT_PIN);
  float current = out * 5.0 / 1.023;
  Serial.print(current);
  Serial.println("mA");
  delay(1000);       
}
