#include "ButtonPoll_CW.h"

ButtonPoll_CW btn;
const byte BUTTON_PIN = 7;

void setup()
{
  // Init Serial
  Serial.begin(9600);
  
  // Init button
  btn.begin(BUTTON_PIN);
}

void loop()
{
  if (btn.pressed())
    Serial.println("Button Pressed");
}
