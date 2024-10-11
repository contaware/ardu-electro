/*
  DC voltage measurement with an external LM4040 voltage reference
  and a voltage divider
  
  - The Adafruit precision LM4040 voltage reference breakout has
    2.048V and 4.096V references to be used with 3.3V and 5V MCUs.
    Just power with 5V - 12V, or with 3.3V if you want only the 
    2.048V reference to work.

  - Here we use a common voltage divider breakout with a 30k resistor
    between the input and the output and a 7.5k resistor between the
    output and the ground. This gives a 5x divider and a measurement
    range of 0 - 5*Vref(=20.48V). Note that in the breakout there is
    a + output terminal which is not connected.
*/
 
// Define analog input pin
#define ANALOG_IN_PIN A0
 
// Constants for resistor values in divider (in ohms)
const float R1 = 30000.0;
const float R2 = 7500.0;
const float VD_RATIO = (R1 + R2) / R2;
 
// Reference Voltage (4.096 or 2.048)
const float AREF_VOLT = 4.096;
 
void setup()
{
  // Serial Debug
  Serial.begin(9600);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!

  // Use external voltage reference
  analogReference(EXTERNAL);
}
 
void loop()
{  
  // Voltage at ADC input
  // https://skillbank.co.uk/arduino/measure.htm
  // https://skillbank.co.uk/arduino/adc.htm
  float adcVoltage  = ((float)analogRead(ANALOG_IN_PIN) + 0.5) * AREF_VOLT / 1024.0;
   
  // Voltage at divider input
  float realVoltage = adcVoltage * VD_RATIO; 
   
  // Print
  Serial.print("Input Voltage = ");
  Serial.print(realVoltage, 2);
  Serial.println(" V");
  
  delay(500);
}
