#include "ButtonPoll.h"

ButtonPoll btn;

void setup()
{
  Serial.begin(9600);
  btn.begin(10);    // switch pin
}

void loop()
{
  if (btn.pressed())
    Serial.println("Button Pressed");
}
