/* 
  Drive a DC motor with two Arduino pins

  A. Using the TB67H450FNG/A4950/AT8236 drivers
  
  - The TB67H450FNG/A4950/AT8236 chips have one H-bridges with no 
    ENABLE/PWM pin. IN1 and IN2 are logic input signals between 3.3V - 5.25V.

  - The MOSFET driver's body diodes are usually enough to protect the 
    MOSFET from the reverse voltage peak that occurs across the motor 
    coil when switching the motor off.

  - The TB67H450FNG/A4950/AT8236 chips have a current limiting circuitry 
    which works through a sense resistor attached between the RS pin and GND. 
    Max current is calculated like: 
    Iout(max) = VREF / Rsense / 10 = 3.3 / 0.250 / 10 = 1.3A

                     ----u----
                GND |1       8| OUT2
    IN2_PIN <-> IN2 |2       7| RS <-> Rsense(0.250Ω) <-> GND
    IN1_PIN <-> IN1 |3       6| OUT1
      3.3V <-> VREF |4       5| VM (VCC motor)
                     ---------
            TB67H450FNG/A4950/AT8236 (3A)

    TB67H450FNG:  VREF=0-4V        VM=4.5-44V
    A4950:        VREF=0-5V        VM=8-40V
    AT8236:       VREF=0.5-4V      VM=5.5-36V
*/
#define IN1_PIN       5
#define IN2_PIN       6

void setup()
{
  pinMode(IN1_PIN, OUTPUT);
}

void loop()
{
  // 1. Back and forth
  for (int i = 0 ; i < 2 ; i++)
  {
    // One direction
    digitalWrite(IN1_PIN, HIGH);
    analogWrite(IN2_PIN, 0);
    delay(500);
    digitalWrite(IN1_PIN, LOW);
    analogWrite(IN2_PIN, 0);
    delay(500);

    // Other direction
    digitalWrite(IN1_PIN, LOW);
    analogWrite(IN2_PIN, 255);
    delay(500);
    digitalWrite(IN1_PIN, LOW);
    analogWrite(IN2_PIN, 0);
    delay(500);
  }

  delay(2000);
  
  // 2. Stop by setting both LOW
  // - TB67H450FNG/A4950/AT8236: motor will free spin because outputs are high-Z.
  digitalWrite(IN1_PIN, HIGH);
  analogWrite(IN2_PIN, 0);
  delay(3000);
  digitalWrite(IN1_PIN, LOW);
  analogWrite(IN2_PIN, 0);
  
  delay(2000);

  // 3. Stop by setting both HIGH
  // - TB67H450FNG/A4950/AT8236: motor will brake because outputs are both LOW.
  digitalWrite(IN1_PIN, HIGH);
  analogWrite(IN2_PIN, 0);
  delay(3000);
  digitalWrite(IN1_PIN, HIGH);
  analogWrite(IN2_PIN, 255);
  
  delay(2000);

  // 4. Speed control
  // - When both IN1 and IN2 are LOW for more than 1ms, the device will enter 
  //   standby mode. According to the TB67H450FNG datasheet we should avoid 
  //   changing IN1 and IN2 during the standby transition period, which is 
  //   between 0.7ms - 1.5ms. The Arduino's PWM frequency falls within this 
  //   range, so to avoid issues with standby mode, set one pin to HIGH and 
  //   use the other as PWM pin.
  digitalWrite(IN1_PIN, HIGH);
  analogWrite(IN2_PIN, 0);
  delay(2000);
  analogWrite(IN2_PIN, 255 - 180);
  delay(2000);
  analogWrite(IN2_PIN, 255 - 128);
  delay(2000);
  analogWrite(IN2_PIN, 255 - 64);
  delay(2000);
  analogWrite(IN2_PIN, 255 - 128);
  delay(2000);
  analogWrite(IN2_PIN, 255 - 180);
  delay(2000);
  analogWrite(IN2_PIN, 255 - 255);
  delay(2000);
  digitalWrite(IN1_PIN, LOW);
  analogWrite(IN2_PIN, 0);

  delay(2000);
}
