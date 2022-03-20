/*
  ACS712 current measurement through Hall Effect
 
  - VDD supply is 4.5V – 5.5V.
  - In the ACS712 the current to be measured generates a magnetic field
    which is converted into a voltage, that can be picked-up at the Out pin.
    Due to its measuring principle, the ACS712 influences the current to be
    measured less than a shunt-based modules. However the precision is not
    so good.
  - There are 3 chip types:
    ACS712T-ELC-05B: 185mV/A and max 5A  
    ACS712T-ELC-20A: 100mV/A and max 20A 
    ACS712T-ELC-30A:  66mV/A and max 30A 
  - Vout = VDD/2 at 0A and changes by the specified amount of mV/A
    For example for the 5A chip:
    Vout = 0.185 * I + VDD/2  ->  I = (Vout - VDD/2) / 0.185​
*/

const byte ACS712T_ELC_05B_PIN = A0;
const byte ACS712T_ELC_20A_PIN = A1;
const byte ACS712T_ELC_30A_PIN = A2;

const int zeroCurrent05B = 510; // test that!
const int zeroCurrent20A = 510; // test that!
const int zeroCurrent30A = 510; // test that!

void setup()
{
  Serial.begin(9600);
  Serial.println("ACS712 Test");
  Serial.println();
}

void loop()
{
  int raw05B = analogRead(ACS712T_ELC_05B_PIN);
  int raw20A = analogRead(ACS712T_ELC_20A_PIN);
  int raw30A = analogRead(ACS712T_ELC_30A_PIN);
  
  float current05B = (raw05B - zeroCurrent05B) * 5.0 / 1.023 / 0.185;
  float current20A = (raw20A - zeroCurrent20A) * 5.0 / 1.023 / 0.100;
  float current30A = (raw30A - zeroCurrent30A) * 5.0 / 1.023 / 0.066;
  
  Serial.print("ACS712T-ELC-05B: ");
  Serial.print(raw05B);
  Serial.print(" -> ");
  Serial.print(current05B);
  Serial.println("mA");

  Serial.print("ACS712T-ELC-20A: ");
  Serial.print(raw20A);
  Serial.print(" -> ");
  Serial.print(current20A);
  Serial.println("mA");

  Serial.print("ACS712T-ELC-30A: ");
  Serial.print(raw30A);
  Serial.print(" -> ");
  Serial.print(current30A);
  Serial.println("mA");
  
  Serial.println();
  
  delay(1000);        
}
