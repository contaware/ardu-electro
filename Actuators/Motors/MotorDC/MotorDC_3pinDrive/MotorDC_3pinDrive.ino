/* 
  Drive a DC motor with three Arduino pins

  A. Using the L293D/SN754410 drivers
  
  - The L293D/SN754410 chips have 4X half H-bridges with two enable pins. 
    Each enable pin controls two half H-bridges. To drive a motor 
    bidirectionally we use two half H-bridges.

  - The diodes in the L293D/SN754410 chips protect the transistors from 
    the reverse voltage peak that occurs across the motor coil when 
    switching the motor off (L293 without ending D has no diodes).

                             -----u-----
    EN_PWM_PIN <-> ENABLE12 |1        16| VCC signal (4.5V - 5.5V)
            DIR1_PIN <-> 1A |2        15| 4A
              MOTOR+ <-> 1Y |3        14| 4Y
                        GND |4        13| GND
                        GND |5        12| GND
              MOTOR- <-> 2Y |6        11| 3Y
            DIR2_PIN <-> 2A |7        10| 3A
     VCC motor (4.5V - 36V) |8         9| ENABLE34
                             -----------
                      L293D (600mA per channel)
                      SN754410 (1A per channel)


  B. Using the L298 driver

  - The L298 chip has two H-bridges with two enable pins. 
    Each enable pin controls a H-bridge.

  - The L298 module comes with the needed protection diodes,
    and has a jumper-enabled 78M05 regulator to provide the 
    VCC signal.

                            ----------
        EN_PWM_PIN <-> ENA |          | OUT1 <-> MOTOR+
          DIR1_PIN <-> IN1 |  Module  | OUT2 <-> MOTOR-
          DIR2_PIN <-> IN2 |          | VCC motor (5V - 35V)
    VCC signal (4.5V - 7V) |          | GND
                            ----------
                      L298 (2A per channel)


  C. Using the BTS7960 driver

  - This MOSFET based driver chip contains a half H-bridge with 
    a INH (inhibit, same as enable) pin and a IN pin.
    
  - The module brings two BTS7960 chips and has a 74HC244 buffer.

                                 ----------
            EN_PWM_PIN <-> L_EN |          | M+ <-> MOTOR+
            EN_PWM_PIN <-> R_EN |          | M- <-> MOTOR-
              DIR1_PIN <-> LPWM |  Module  | B+ (5.5V - 27.5V)
              DIR2_PIN <-> RPWM |          | B- (connected to GND)
    VCC for 74HC244 (3.3V - 5V) |          | GND
                                 ----------
                                BTS7960 (43A)


  D. Using the TB6612FNG driver

  - The TB6612FNG chip has two H-bridges with two PWM pins.
    
  - The MOSFET driver's body diodes are usually enough to protect the 
    MOSFET from the reverse voltage peak that occurs across the motor 
    coil when switching the motor off.
    
                    -------u-------
    MOTOR+ <-> AO1 |1            24| VCC motor (2.5V - 13.5V)
    MOTOR+ <-> AO1 |2            23| PWMA <-> EN_PWM_PIN
               GND |3            22| AIN2 <-> DIR2_PIN
               GND |4            21| AIN1 <-> DIR1_PIN
    MOTOR- <-> AO2 |5            20| VCC signal (2.7V - 5.5V)
    MOTOR- <-> AO2 |6            19| STBY <-> set HIGH
               BO2 |7            18| GND signal
               BO2 |8            17| BIN1
               GND |9            16| BIN2
               GND |10           15| PWMB
               BO1 |11           14| VCC motor (2.5V - 13.5V)
               BO1 |12           13| VCC motor (2.5V - 13.5V)
                    ---------------
              TB6612FNG (1A per channel)
*/
#define DIR1_PIN      3
#define DIR2_PIN      4
#define EN_PWM_PIN    5

void setup()
{
  pinMode(DIR1_PIN, OUTPUT);
  pinMode(DIR2_PIN, OUTPUT);
}

void loop()
{
  // 1. Back and forth
  for (int i = 0 ; i < 2 ; i++)
  {
    // One direction
    analogWrite(EN_PWM_PIN, 255);
    digitalWrite(DIR1_PIN, HIGH);
    digitalWrite(DIR2_PIN, LOW);
    delay(500);
    analogWrite(EN_PWM_PIN, 0);
    delay(500);

    // Other direction
    analogWrite(EN_PWM_PIN, 255);
    digitalWrite(DIR1_PIN, LOW);
    digitalWrite(DIR2_PIN, HIGH);
    delay(500);
    analogWrite(EN_PWM_PIN, 0);
    delay(500);
  }

  delay(2000);
  
  // 2. Stop by setting EN_PWM_PIN LOW
  // - L293D/SN754410/L298/BTS7960: motor will free spin because 
  //   outputs are high-Z.
  // - TB6612FNG: motor will brake because outputs are both LOW.
  analogWrite(EN_PWM_PIN, 255);
  digitalWrite(DIR1_PIN, HIGH);
  digitalWrite(DIR2_PIN, LOW);
  delay(3000);
  analogWrite(EN_PWM_PIN, 0);
  
  delay(2000);

  // 3. Stop by setting DIR1_PIN and DIR2_PIN LOW
  // - L293D/SN754410/L298/BTS7960: motor will brake because 
  //   outputs are both LOW, but note that the BJT output stage 
  //   of the L293D/SN754410/L298 is not so effective at shorting 
  //   the motor like the MOSFET output stage of the BTS7960.
  // - TB6612FNG: motor will free spin because outputs are high-Z.
  analogWrite(EN_PWM_PIN, 255);
  digitalWrite(DIR1_PIN, HIGH);
  digitalWrite(DIR2_PIN, LOW);
  delay(3000);
  digitalWrite(DIR1_PIN, LOW);
  digitalWrite(DIR2_PIN, LOW);
  
  delay(2000);
  
  // 4. Speed control
  analogWrite(EN_PWM_PIN, 255);
  digitalWrite(DIR1_PIN, HIGH);
  digitalWrite(DIR2_PIN, LOW);
  delay(2000);
  analogWrite(EN_PWM_PIN, 180);
  delay(2000);
  analogWrite(EN_PWM_PIN, 128);
  delay(2000);
  analogWrite(EN_PWM_PIN, 64);
  delay(2000);
  analogWrite(EN_PWM_PIN, 128);
  delay(2000);
  analogWrite(EN_PWM_PIN, 180);
  delay(2000);
  analogWrite(EN_PWM_PIN, 255);
  delay(2000);
  analogWrite(EN_PWM_PIN, 0);

  delay(2000);
}
