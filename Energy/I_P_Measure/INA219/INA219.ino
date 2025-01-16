/*
  INA219 current and power measurement with a shunt resistor
 
  - Can only do high-side current sensing.

  - Vs supply is 3V – 5.5V.
  
  - Load supply can be 0V - 26V.
  
  - The current to be measured is passed through a shunt resistor and 
    the voltage across it is determined with an ADC.
    The chip also calculates the power consumption of the consumer by 
    measuring the voltage drop across the consumer. The circuit of shunt 
    and consumer is separated from the supply current of the chip, but 
    it's important that the two have a common GND.

  - Max load current with the 0.1Ω (R100) shunt is ±3.2A with a 
    0.8mA resolution. Note: the used library does not support other
    shunt values.

  - The chip has two address pins, A0 and A1, by applying 4 different 
    signals (GND, Vs, SDA, SCL) we get 16 I2C addresses (0x40-0x4F).
*/
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219(0x40); // default is 0x40

void setup()
{
  // Init Serial (leave Serial Monitor open to see all messages)
  Serial.begin(9600); delay(5000); // wait 5s that Serial is ready
  Serial.println("INA219 Test");
  
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
  
  // To use a slightly lower 32V, 1A range (higher precision on current):
  //ina219.setCalibration_32V_1A();
  
  // Or to use a lower 16V, 400mA range (higher precision on voltage and current):
  //ina219.setCalibration_16V_400mA();
}

void loop()
{
  float busVoltage_V = ina219.getBusVoltage_V();
  float shuntVoltage_mV = ina219.getShuntVoltage_mV();
  float loadVoltage_V = busVoltage_V + (shuntVoltage_mV / 1000);
  float current_mA = ina219.getCurrent_mA();
  float power_mW = ina219.getPower_mW();
  
  Serial.print("Bus Voltage:   "); Serial.print(busVoltage_V); Serial.println("V");
  Serial.print("Shunt Voltage: "); Serial.print(shuntVoltage_mV); Serial.println("mV");
  Serial.print("Load Voltage:  "); Serial.print(loadVoltage_V); Serial.println("V");
  Serial.print("Current:       "); Serial.print(current_mA); Serial.println("mA");
  Serial.print("Power:         "); Serial.print(power_mW); Serial.println("mW"); // power = busvoltage * current
  Serial.println();

  delay(1000);
}
