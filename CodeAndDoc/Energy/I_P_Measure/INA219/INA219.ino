/*
  INA219 current and power measurement with a shunt resistor
 
  - VDD supply is 3V – 5.5V.
  
  - Load supply can be 0V - 26V.
  
  - Max load current when using a 0.1 ohm (R100) shunt is ±3.2A with a ±0.8mA resolution.
  
  - The current to be measured is passed through the shunt of 0.1 ohm 
    and the voltage across it is determined with an ADC.
    The INA219 also calculates the power consumption of the consumer by 
    measuring the voltage drop across the consumer. The circuit of shunt and 
    consumer is separated from the supply current of the INA219, but it's 
    important that the two have a common GND.
*/
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;

void setup()
{
  Serial.begin(9600);
  
  // Initialize the INA219
  // By default the initialization will use the largest range (32V, 2A).
  // However you can call on of the setCalibration functions to change this.
  // Note: the 32V parameter only expresses the full-scale range of the ADC
  //       scaling. In no event should more than 26V be applied to this device!
  if (!ina219.begin())
  {
    Serial.println("Failed to find the INA219 chip");
    while (true);
  }
  
  // To use a slightly lower 32V, 1A range (higher precision on amps):
  //ina219.setCalibration_32V_1A();
  
  // Or to use a lower 16V, 400mA range (higher precision on volts and amps):
  //ina219.setCalibration_16V_400mA();

  Serial.println("INA219 Test");
  Serial.println();
}

void loop()
{
  float busvoltage = ina219.getBusVoltage_V();
  float shuntvoltage = ina219.getShuntVoltage_mV();
  float loadvoltage = busvoltage + (shuntvoltage / 1000);
  float current_mA = ina219.getCurrent_mA();
  float power_mW = ina219.getPower_mW();
  
  Serial.print("Bus Voltage:   "); Serial.print(busvoltage); Serial.println(" V");
  Serial.print("Shunt Voltage: "); Serial.print(shuntvoltage); Serial.println(" mV");
  Serial.print("Load Voltage:  "); Serial.print(loadvoltage); Serial.println(" V");
  Serial.print("Current:       "); Serial.print(current_mA); Serial.println(" mA");
  Serial.print("Power:         "); Serial.print(power_mW); Serial.println(" mW");  // power = busvoltage * current
  
  Serial.println();

  delay(1000);
}
