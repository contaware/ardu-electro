#include "RotaryEncoderPoll.h"
#include "ButtonPoll.h"

RotaryEncoderPoll enc;
ButtonPoll btn;

const byte ROTARY_ENCODER_CLK_PIN = 8;
const byte ROTARY_ENCODER_DT_PIN = 9;
const byte ROTARY_ENCODER_SW_PIN = 7;

long count = 0;

void setup()
{
  Serial.begin(9600);
  enc.begin(ROTARY_ENCODER_CLK_PIN, ROTARY_ENCODER_DT_PIN);
  btn.begin(ROTARY_ENCODER_SW_PIN);
}

void loop()
{
  // Read rotary encoder
  int res = enc.read();
  if (res == 1)
  {
    count++;
    Serial.println(count);
  }
  else if (res == -1)
  {
    count--;
    Serial.println(count);
  }

  // On button press do reset the count
  if (btn.pressed())
  {
    count = 0;
    Serial.println("Count reset");
  }
}
