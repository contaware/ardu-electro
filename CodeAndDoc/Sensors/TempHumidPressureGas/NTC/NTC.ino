/*
  NTC (Negative temperature coefficient) thermistor

  - Resistance decreases as temperature rises.

  - An NTC is commonly used as a temperature sensor.
  
    VCC
     |
    .-.
    | | R1
    | | 10kΩ
    '-'
     |
     o---------o A0
     |
    .-.
    | | R
    | | NTC
    '-'
     |
    GND
*/
const byte V_PIN = A0;        // analog pin to read V
const float R1 = 10000;       // in Ω

// For these constants consult the NTC datasheet
// Hint: if you do not have the beta or Ro, you can calculate them 
//       with two measurements:
//       https://www.giangrandi.org/electronics/ntc/ntc.shtml
const float Ro = 10000;       // NTC nominal resistance in Ω
const float To = 25 + 273.15; // NTC nominal resistance at this temperature in °K
const float beta = 3950;      // NTC beta coefficient, also called B value

void setup()
{
  Serial.begin(9600);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!
}

void loop()
{
  // NTC resistance measurement
  // Equation 1:
  //   1/I = R/V = R1 / (VCC - V)
  //   -> R = V * R1 / (VCC - V) = R1 / (VCC/V - 1)
  // Equation 2:
  //   V = analogRead(V_PIN) / 1023.0 * VCC 
  //   -> VCC/V = 1023.0 / analogRead(V_PIN)
  // Equation 2 in 1 gives the thermistor resistance:
  float R = R1 / (1023.0 / analogRead(V_PIN) - 1);
  Serial.print("Thermistor "); 
  Serial.print(R); 
  Serial.print("Ω"); 

  // Temperature calculation with B or β parameter equation 
  // https://en.wikipedia.org/wiki/Thermistor#B_or_%CE%B2_parameter_equation
  float temperature = 1 / To + log(R / Ro) / beta;
  temperature = 1 / temperature;  // invert
  temperature -= 273.15;          // convert °K to °C
  Serial.print(" -> Temperature ");
  Serial.print(temperature);
  Serial.println("°C");

  delay(1000);
}
