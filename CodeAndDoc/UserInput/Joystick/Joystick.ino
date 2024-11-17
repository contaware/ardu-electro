/*
  Analog Joystick

  - On 5V MCUs power it with 5V.
  
  - On 3.3V MCUs power it with 3.3V, not with 5V, even if it's marked 
    as such on the module!

  - The switch shorts to GND when pressed.

  - It uses 10kΩ potentiometers for the positions.
  
         VCC                  VCC                 SW
          |                    |                  |
         .-.                  .-.                 o
      Rx | |<---o VRx      Ry | |<---o VRy         \
    10kΩ | |             10kΩ | |                   \
         '-'                  '-'                 o
          |                    |                  |
         GND                  GND                GND
*/
const byte SW_PIN = 2;  // digital pin connected to SW
const byte X_PIN = A0;  // analog pin connected to VRx
const byte Y_PIN = A1;  // analog pin connected to VRy

void setup()
{
  // Init Serial
  Serial.begin(9600);
  
  // Set pin mode
  pinMode(SW_PIN, INPUT_PULLUP);
}

void loop()
{
  // Button
  Serial.print("SW=");
  Serial.print(digitalRead(SW_PIN));
  Serial.print(" ");

  // X-axis
  Serial.print("X=");
  Serial.print(analogRead(X_PIN));
  Serial.print(" ");
  
  // Y-axis
  Serial.print("Y=");
  Serial.println(analogRead(Y_PIN));

  delay(300);
}
