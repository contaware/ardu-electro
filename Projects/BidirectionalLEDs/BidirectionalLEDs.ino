/* 
  Use a H-bridge to drive LEDs bidirectionally

  - To fully illuminate a light chain/string made of LEDs in alternating 
    polarity, we have to fast switch the polarity so that the eye does 
    not see a flickering.

  - The dimming regulation is achieved through a 10kΩ potentiometer.

  - Do not forget an output resistor to limit the current into the LEDs. 
    Even better would be a H-bridge with current limitation.
*/
#define DIR1_PIN        3
#define DIR2_PIN        4
#define EN_PWM_PIN      5
#define REGULATION_PIN  A0
#define MIN_PWM         8     // 0..255
#define MAX_PWM         160   // 0..255

void setup()
{
  pinMode(DIR1_PIN, OUTPUT);
  pinMode(DIR2_PIN, OUTPUT);
}

void loop()
{ 
  int value = analogRead(REGULATION_PIN);
  value = map(value, 0, 1023, MIN_PWM, MAX_PWM);
  analogWrite(EN_PWM_PIN, value);
  digitalWrite(DIR1_PIN, HIGH);
  digitalWrite(DIR2_PIN, LOW);
  delay(4);
  digitalWrite(DIR1_PIN, LOW);
  digitalWrite(DIR2_PIN, HIGH);
  delay(4);
}
