/*
  Control an analog RGB led

  - RGB LEDs have three LEDs inside them which share either a 
    common anode or a common cathode (longest lead).

  - Connect the common cathode to GND, or if it's a common anode,
    connect it to VCC.

  - Connect each color through a 330Ω resistor to the respective
    analog pin defined below.

      _---_
     | RGB |
    |  LED  |
    |_______|
     | | | |
     B | | |
       G | R
        K/A
*/
#define BLUE_PIN        3  // PWM pin
#define GREEN_PIN       5  // PWM pin
#define RED_PIN         6  // PWM pin

#define DELAY_TIME_MS   15
#define DELAY_WAIT_MS   2000

void setup()
{

}

void loop()
{
  for (int i = 0; i <= 255; i++)
  {
    analogWrite(RED_PIN, 255 - i);
    analogWrite(GREEN_PIN, i);
    delay(DELAY_TIME_MS);
  }
  delay(DELAY_WAIT_MS);
  
  for (int i = 0; i <= 255; i++)
  {
    analogWrite(GREEN_PIN, 255 - i);
    analogWrite(BLUE_PIN, i);
    delay(DELAY_TIME_MS);
  }
  delay(DELAY_WAIT_MS);

  for (int i = 0; i <= 255; i++)
  {
    analogWrite(BLUE_PIN, 255 - i);
    analogWrite(RED_PIN, i);
    delay(DELAY_TIME_MS);
  }
  delay(DELAY_WAIT_MS);
}
