const byte SW_PIN = 2;  // digital pin connected to switch output
const byte X_PIN = 0;   // analog pin connected to X output
const byte Y_PIN = 1;   // analog pin connected to Y output

void setup()
{
  pinMode(SW_PIN, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop()
{
  Serial.print("Switch:  ");
  Serial.print(digitalRead(SW_PIN));
  Serial.print("\n");
  Serial.print("X-axis: ");
  Serial.print(analogRead(X_PIN));
  Serial.print("\n");
  Serial.print("Y-axis: ");
  Serial.println(analogRead(Y_PIN));
  Serial.print("\n\n");
  delay(500);
}
