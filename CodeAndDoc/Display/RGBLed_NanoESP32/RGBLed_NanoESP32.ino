/*
  Nano ESP32 board's RGB led

  - to turn ON a led, set its digital pin LOW.

  - a limited number of Arduino Nano ESP32 boards were assembled
    with an RGB LED which has the green and blue pins inverted.
*/

void setup()
{
  Serial.begin(9600);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  delay(5000);
  Serial.println("The digital pins for the built-in RGB led are:");
  Serial.print("Red=  D"); Serial.println(LED_RED);
  Serial.print("Green=D"); Serial.println(LED_GREEN);
  Serial.print("Blue= D"); Serial.println(LED_BLUE);
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
