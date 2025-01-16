/*
  INA226/INA230/INA231 current and power measurement with a shunt resistor
 
  - Can do both high-side and low-side current sensing because the Vbus 
    is separate and not internally connected to IN- like for the INA219.

  - Vs supply is 2.7V – 5.5V.

  - Load supply can be 0V - 36V (0V - 28V for the INA231).
  
  - The current to be measured is passed through a shunt resistor and 
    the voltage across it is determined with an ADC.
    The chip also calculates the power consumption of the consumer by 
    measuring the voltage drop across the consumer. The circuit of shunt 
    and consumer is separated from the supply current of the chip, but 
    it's important that the two have a common GND.

  - The shunt voltage ADC has a resolution of 2.5μV/bit. The Shunt 
    Voltage Register returns 16-bit two's complement numbers. By using a 
    0.1Ω (R100) shunt, we can measure from -(2^15) * 2.5μV / 0.1Ω = -0.8192A 
    to (2^15-1) * 2.5μV / 0.1Ω = 0.819175A with a resolution of 25μA.

  - For the bus voltage we have 1.25mV/bit, so that we can measure up to 
    (2^15-1) * 1.25mV = 40.95875V.
    Attention: that only expresses the full-scale range of the ADC. In 
               no event should more than 36V (28V for the INA231) be 
               applied to this device!

  - We can directly read the shunt and bus registers and make our 
    calculations for current and power with the MCU, or use the current 
    and power registers calculated by the chip. The chip let us 
    configure the shunt value and the current range so that it can fit 
    current and power in its 16-bit registers.

  - The chip has two address pins, A0 and A1, by applying 4 different 
    signals (GND, Vs, SDA, SCL) we get 16 I2C addresses (0x40-0x4F).
*/
#include <INA226_WE.h>  // by Wolfgang Ewald
                        // Note: library works also for INA230 and INA231
                        // https://wolles-elektronikkiste.de/en/ina226-current-and-power-sensor
   
INA226_WE ina226(0x40); // default is 0x40

void setup()
{
  // Init Serial (leave Serial Monitor open to see all messages)
  Serial.begin(9600); delay(5000); // wait 5s that Serial is ready
  Serial.println("INA226 Test");
  
  // Initialize the INA226
  Wire.begin();
  if (!ina226.init())
  {
    Serial.println("Failed to find the INA226 chip");
    while (true);
  }
  
  /* 
    Set the number of measurements which shall be averaged
    (holds for both shunt and bus voltage)
    AVERAGE_1            1 (default)
    AVERAGE_4            4
    AVERAGE_16          16
    AVERAGE_64          64
    AVERAGE_128        128
    AVERAGE_256        256
    AVERAGE_512        512
    AVERAGE_1024      1024
  */
  ina226.setAverage(AVERAGE_64);

  /* 
    Set conversion time in microseconds
    Shunt + bus voltage conversion will take: 
    number of samples to be averaged * conversion time * 2

    CONV_TIME_140       140 µs
    CONV_TIME_204       204 µs
    CONV_TIME_332       332 µs
    CONV_TIME_588       588 µs
    CONV_TIME_1100      1.1 ms (default)
    CONV_TIME_2116    2.116 ms
    CONV_TIME_4156    4.156 ms
    CONV_TIME_8244    8.244 ms  
  */
  ina226.setConversionTime(CONV_TIME_1100);
  
  /*
    Set the measurement mode
    POWER_DOWN        INA226 switched off
    TRIGGERED         measurement on demand
    CONTINUOUS        continuous measurements (default)
  */
  ina226.setMeasureMode(CONTINUOUS);
  
  /* 
    Set Resistor and Current Range
    default is 0.1Ω and 0.8192A
  */
  //ina226.setResistorRange(0.005, 10.0); // choose 5mΩ resistor and a range of 10A

  /*
    If the current values delivered by the INA226 differ by a 
    constant factor from values obtained with calibrated equipment,
    you can define a correction factor like:
    Correction factor = current delivered from calibrated equipment / 
                        current delivered by INA226
  */
  //ina226.setCorrectionFactor(0.95);
  
  // Wait for the first data
  ina226.waitUntilConversionCompleted();
}

void loop()
{
  float busVoltage_V = ina226.getBusVoltage_V();
  float shuntVoltage_mV = ina226.getShuntVoltage_mV();
  float loadVoltage_V = busVoltage_V + (shuntVoltage_mV / 1000);
  float current_mA = ina226.getCurrent_mA();
  float power_mW = ina226.getBusPower();
  
  if (shuntVoltage_mV < -81.9 || shuntVoltage_mV > 81.9) Serial.println("OUT OF RANGE!");
  Serial.print("Bus Voltage:   "); Serial.print(busVoltage_V); Serial.println("V");
  Serial.print("Shunt Voltage: "); Serial.print(shuntVoltage_mV, 4); Serial.println("mV");
  Serial.print("Load Voltage:  "); Serial.print(loadVoltage_V); Serial.println("V");
  Serial.print("Current:       "); Serial.print(current_mA, 3); Serial.println("mA");
  Serial.print("Power:         "); Serial.print(power_mW); Serial.println("mW"); // power = busvoltage * current
  Serial.println();

  delay(1000);
}
