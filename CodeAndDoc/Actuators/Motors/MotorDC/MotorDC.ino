/* 
  Drive a DC motor with the L293D/SN754410 chip

  - The L293D/SN754410 chip has 4X half H-bridges with two enable pins. 
    Each enable pin controls two half H-bridges. To drive a motor 
    bidirectionally we use two half H-bridges.
  
  - It's a TTL logic chip, so it works at 5V. The output stage can drive 
    5V-36V motors up to 600mA for the L293D and up to 1A for the SN754410.

  - The diodes in the L293D/SN754410 chip protect the transistors from 
    the reverse voltage peak that occurs across the motor coil when 
    switching the motor off.
    Attention: the L293 without ending D has no diodes.

  - Connect the L293D/SN754410 chip to the Arduino and the motor like:
               -----u-----
       ENABLE |1        16| 5V
         DIRA |2        15|
       MOTOR+ |3        14|
          GND |4        13| GND
          GND |5        12| GND
       MOTOR- |6        11|
         DIRB |7        10|
    VCC MOTOR |8         9|
               -----------

  - Drivers considerations:
    1. DC motors can also be driven by a single transistor in low-side or
       high-side configuration (remember the clamping diode across the motor).
    2. The advantage of a half H-bridge as opposed to a single transistor 
       configuration is that it can brake. That's because a half H-bridge can 
       source and sink current and thus it can short-circuit the motor. When 
       the motor is free spinning it acts as a generator and shorting it will 
       brake because current is consumed. Note that with the L293D/SN754410 
       drivers you will not see a brake effect, this comes from the fact that 
       their bipolar output stage is not effective at shorting the motor like 
       a MOSFET would be.
    3. To be able to reverse the direction, we need a full H-bridge like used 
       in this example. With a full H-bridge it's possible to brake by 
       reversing the direction for a short moment.
*/
#define ENABLE_PIN    5
#define DIRA_PIN      3
#define DIRB_PIN      4

void setup()
{
  pinMode(ENABLE_PIN, OUTPUT);
  pinMode(DIRA_PIN, OUTPUT);
  pinMode(DIRB_PIN, OUTPUT);
}

void loop()
{
  // 1. Back and forth
  digitalWrite(ENABLE_PIN, HIGH);
  for (int i = 0 ; i < 5 ; i++)
  {
    digitalWrite(DIRA_PIN, HIGH);
    digitalWrite(DIRB_PIN, LOW);
    delay(500);
    digitalWrite(DIRA_PIN, LOW);
    digitalWrite(DIRB_PIN, HIGH);
    delay(500);
  }
  digitalWrite(ENABLE_PIN, LOW);

  delay(2000);
  
  // 2. Stop leaving the motor spin
  digitalWrite(ENABLE_PIN, HIGH);
  digitalWrite(DIRA_PIN, HIGH);
  digitalWrite(DIRB_PIN, LOW);
  delay(3000);
  digitalWrite(ENABLE_PIN, LOW);
  
  delay(2000);

  // 3. Faster stop setting DIRA_PIN = DIRB_PIN
  digitalWrite(ENABLE_PIN, HIGH);
  digitalWrite(DIRA_PIN, HIGH);
  digitalWrite(DIRB_PIN, LOW);
  delay(3000);
  digitalWrite(DIRA_PIN, LOW);
  digitalWrite(DIRB_PIN, LOW);
  
  delay(2000);
  
  // 4. Fastest stop by reversing direction
  digitalWrite(DIRA_PIN, HIGH);
  digitalWrite(DIRB_PIN, LOW);
  delay(3000);
  digitalWrite(DIRA_PIN, LOW);
  digitalWrite(DIRB_PIN, HIGH);
  delay(300); // that time depends from the motor type
  digitalWrite(DIRB_PIN, LOW);
  
  delay(2000);
  
  // 5. PWM example, full speed then slow
  digitalWrite(DIRA_PIN, HIGH);
  digitalWrite(DIRB_PIN, LOW);
  delay(2000);
  analogWrite(ENABLE_PIN, 180);
  delay(2000);
  analogWrite(ENABLE_PIN, 128);
  delay(2000);
  analogWrite(ENABLE_PIN, 64);
  delay(2000);
  analogWrite(ENABLE_PIN, 128);
  delay(2000);
  analogWrite(ENABLE_PIN, 180);
  delay(2000);
  analogWrite(ENABLE_PIN, 255);
  delay(2000);
  digitalWrite(ENABLE_PIN, LOW);

  delay(2000);
}
