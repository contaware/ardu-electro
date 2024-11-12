/*
  BlackPill Test

  - The LED is connected between PC13 (=LED_BUILTIN) and 3.3V.

  - The key button is connected between PA0 (=USER_BTN) and GND.
    Note: all GPIOs have internal pull-ups and pull-downs of 40kΩ.
  
  - Analog pins are A0..A9 (we can also use PA0..PB1), the resolution 
    is 12-bit.
    Attention: A0 is PA0 and it is used by the key button!
*/

void setup()
{
  /* 
    Init Serial
    - To have Serial from USART1 (where your USB to TTL adapter is connected):
      Set "USB support" to "None" and "U(S)ART support" to "Enabled (generic Serial)"

    - To have Serial from USB:
      Set "USB support" to "CDC (generic Serial supersede U(S)ART)"
  */
  Serial.begin(9600);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!

  // Init pins
  pinMode(USER_BTN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);

  // Set resolution to 12-bit (Arduino defaults to 10-bit)
  analogReadResolution(12);

  // We can set the analogWrite() PWM frequency common to all channels of a specified timer
  analogWriteFrequency(PWM_FREQUENCY); // default is PWM_FREQUENCY which is 1000 Hz
}

void loop()
{
  // Read key button state
  if (digitalRead(USER_BTN) == LOW)
  {
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
  }
  else
  {
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
  }

  // Print analog value of A1 pin
  int analogValue = analogRead(A1); //  0..4095
  Serial.print(analogValue);
  Serial.print(" = ");
  Serial.print(3.3 * analogValue / 4096.0);
  Serial.println("V");
}
