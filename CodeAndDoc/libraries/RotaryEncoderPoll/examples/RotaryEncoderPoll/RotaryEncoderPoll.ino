#include "RotaryEncoderPoll.h"
#include "ButtonPoll.h"

RotaryEncoderPoll enc;
ButtonPoll btn;

long count = 0;

void setup()
{
  Serial.begin(9600);
  enc.begin(8, 9);  // CLK and DT pins
  btn.begin(10);    // SW pin
}

void loop()
{
  // Read rotary encoder
  int res = enc.read();
  if (res == 1)
  {
    count++;
    Serial.print("CW  , ");
    Serial.println(count);
  }
  else if (res == -1)
  {
    count--;
    Serial.print("CCW , ");
    Serial.println(count);
  }

  // On button press do reset the count
  if (btn.pressed())
  {
    count = 0;
    Serial.println("Count reset");
  }
}
