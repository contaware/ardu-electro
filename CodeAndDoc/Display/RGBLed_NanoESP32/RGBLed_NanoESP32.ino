/*
  Nano ESP32 board's RGB led

  - To turn ON a led, set its digital pin LOW.

  - A limited number of Arduino Nano ESP32 boards were assembled
    with an RGB LED which has the green and blue pins inverted.
*/

void setup()
{
  // These LED pins are not accessible on the headers of the board, 
  // they can only be used for the RGB LED
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
}

void setRGB(bool redOn, bool greenOn, bool blueOn)
{
  digitalWrite(LED_RED, redOn ? LOW : HIGH);
  digitalWrite(LED_GREEN, greenOn ? LOW : HIGH);
  digitalWrite(LED_BLUE, blueOn ? LOW : HIGH);
}

void loop()
{
  // Red
  setRGB(true, false, false);
  delay(1000);

  // Green
  setRGB(false, true, false);
  delay(1000);
  
  // Blue
  setRGB(false, false, true);
  delay(1000);
}
