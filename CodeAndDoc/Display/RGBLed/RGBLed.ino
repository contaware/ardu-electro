#define BLUE_PIN        3
#define GREEN_PIN       5
#define RED_PIN         6

#define DELAY_TIME_MS   15
#define DELAY_WAIT_MS   1000

void setup()
{
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
}

void loop()
{
  for (int i = 0; i <= 255; ++i)
  {
    analogWrite(RED_PIN, 255 - i);
    //analogWrite(GREEN_PIN, i);
    analogWrite(BLUE_PIN, i);
    delay(DELAY_TIME_MS);
  }
  delay(DELAY_WAIT_MS);
  
  for (int i = 255; i >= 0; --i)
  {
    analogWrite(RED_PIN, 255 - i);
    //analogWrite(GREEN_PIN, i);
    analogWrite(BLUE_PIN, i);
    delay(DELAY_TIME_MS);
  }
  delay(DELAY_WAIT_MS);
}
