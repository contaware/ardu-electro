#include "ButtonPoll.h"

ButtonPoll btn;
const byte BUTTON_PIN = 7;

void setup()
{
  Serial.begin(9600);
  btn.begin(BUTTON_PIN);
}

void loop()
{
  if (btn.pressed())
    Serial.println("Button Pressed");
}
