/* 
  DC motor using the L293D chip
  Note: the diodes in the L293D chip protect the transistors from the reversed 
  voltage peak that will occur if a voltage on a coil is shut off, like what 
  happens in this chip when switching the ENABLE_PIN.
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
  
  // 2. Slow stop example
  digitalWrite(ENABLE_PIN, HIGH);
  digitalWrite(DIRA_PIN, HIGH);
  digitalWrite(DIRB_PIN, LOW);
  delay(3000);
  digitalWrite(ENABLE_PIN, LOW);
  
  delay(2000);
  
  // 3. Fast stop example
  digitalWrite(ENABLE_PIN, HIGH);
  digitalWrite(DIRA_PIN, LOW);
  digitalWrite(DIRB_PIN, HIGH);
  delay(3000);
  digitalWrite(DIRA_PIN, HIGH);
  digitalWrite(DIRB_PIN, LOW);
  delay(300);
  digitalWrite(DIRA_PIN, LOW);
  
  delay(2000);
  
  // 4. PWM example, full speed then slow
  analogWrite(ENABLE_PIN, 255);
  digitalWrite(DIRA_PIN, HIGH);
  digitalWrite(DIRB_PIN, LOW);
  delay(2000);
  analogWrite(ENABLE_PIN, 180);
  delay(2000);
  analogWrite(ENABLE_PIN, 128);
  delay(2000);
  analogWrite(ENABLE_PIN, 50);
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
   
