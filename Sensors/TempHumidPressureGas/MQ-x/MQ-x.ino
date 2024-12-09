/*
  MQ-x gas sensors

  - VDD supply is 5V, usually the sensor needs 160mA or less.

  - The sensor works as a variable resistor (RS) that changes its value 
    according to the concentration of gas. If the concentration is high, 
    the resistance decreases. If the concentration is low, the resistance
    increases. To measure that variation, another resistance (RL) is used
    to form a voltage divider, so that the sensor output voltage is:
    V = VCC * RL / (RS + RL)
    from which we can calculate the sensor resistance:
    RS = VCC * RL / V – RL

  - When first used after a long period of storage (a month or more), 
    the sensors must be fully warmed-up for 24 hours to ensure maximum 
    accuracy. If the sensor has recently been used, it will only take 
    5-10 minutes to fully warm-up. During the warm-up period, the sensor 
    typically reads a too high voltage and then gradually decreases 
    until it stabilizes.

  - The module's digital output is driven by a LM393 comparator, the 
    sensitivity can be adjusted with the potentiometer. When the gas 
    concentration exceeds the threshold, the module outputs a LOW and 
    the status LED illuminates. 

  - Test: most sensors react to lighter gas, just let the gas stream out,
    but never directly into the sensor.

  - Exception: the MQ-7 and MQ-309 sensors need two different voltages 
    for the heater. We can achieve those voltages by PWM controlling a 
    N-MOSFET that switches the low-side (GND) of the gas sensor.

  - Advanced: to get the ppm value we have to:
    1. Determine R0 by measuring RS in clean air. The datasheet graph 
       shows the constant RS/R0 ratio for clean air, use it to calculate 
       R0 = RS / constant.
    2. From The graph in the datasheet we can also see that the relation 
       between ppm and RS/R0 is linear (in the log-log plot). With some 
       math we can calculate the slope and the y-intercept of the line 
       which permits us to relate ppm to RS/R0.

  - References:
    https://www.figaro.co.jp/en/technicalinfo/principle/mos-type.html
    https://www.jaycon.com/understanding-a-gas-sensor/
    https://github.com/miguel5612/MQSensorsLib
*/

#define MQ_ANALOG_PIN   A0
#define MQ_DIGITAL_PIN  2
#define RL              1000    // resistor in Ω attached between analog 
                                // output and GND, check your module for 
                                // that value (usually 1kΩ..10kΩ)

void setup()
{
  // Init Serial
  Serial.begin(9600);

  // Init pin
  pinMode(MQ_DIGITAL_PIN, INPUT);
}

void loop() 
{
  // ADC value
  int value = analogRead(MQ_ANALOG_PIN);

  // RS = RL * VCC / V – RL = RL * VCC / (VCC * value / 1023.0) - RL =
  //    = RL * 1023.0 / value - RL
  float RS = RL * 1023.0 / value - RL;

  // Print
  int threshold = digitalRead(MQ_DIGITAL_PIN);
  Serial.print("ADC="); Serial.print(value);
  Serial.print(" , RS="); Serial.print(RS);
  Serial.print(" , Digital pin="); Serial.println(threshold);

  // Delay
  delay(2000);
}
