/*
  Long press push-button to turn Arduino ON and OFF
 
  - The necessary circuit is shown in Push-button_ON-OFF.png

  - The nice thing about this circuit is that you can completly turn OFF
    your Arduino by just setting the BJT_BASE_CONTROL_PIN to LOW. As an
    alternative you could put the MCU to sleep and wake-up with a button
    triggered interrupt, but that would always use some power.
*/
const byte BJT_BASE_CONTROL_PIN = 5;
const byte SW_STATE_SENSE_PIN = 6;
const unsigned long btnPressOnMs = 150;   // in ms
const unsigned long btnPressOffMs = 1500; // in ms
unsigned long btnPressStartMillis;
bool btnPressed = false;

void setup()
{
  // Keep us ON after btnPressOnMs
  pinMode(BJT_BASE_CONTROL_PIN, OUTPUT);
  digitalWrite(BJT_BASE_CONTROL_PIN, LOW);
  delay(btnPressOnMs);
  digitalWrite(BJT_BASE_CONTROL_PIN, HIGH);

  // To read the push-button state
  pinMode(SW_STATE_SENSE_PIN, INPUT_PULLUP);
  
  // Built-in LED status
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
}

static void pollBtnPowerOff()
{
  if (digitalRead(SW_STATE_SENSE_PIN) == LOW) // pressed?
  {
    unsigned long currentMillis = millis();
    if (!btnPressed)
    {
      btnPressed = true;
      btnPressStartMillis = currentMillis;
    }
    else if (currentMillis - btnPressStartMillis > btnPressOffMs)
    {
      // Turn us OFF
      for (int i = 0 ; i < 5 ; i++)
      {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(70);
        digitalWrite(LED_BUILTIN, LOW);
        delay(70);
      }
      digitalWrite(BJT_BASE_CONTROL_PIN, LOW);
      while (true);   
    }
  }
  else
    btnPressed = false;
}

void loop()
{
  // Poll button for power OFF
  pollBtnPowerOff();

  // Do other things
  // ...
}
