/*
  Serial Print
  
  1. The boards with non-native USB support (UNO R3, Mega, Nano) are 
     restarted each time the Serial Monitor is opened. For those boards 
     a simple Serial.begin() is fine and enough.

  2. For boards with native USB support (UNO R4, Leonardo, MKR, Nano 33)
     the Serial Port runs over USB, and opening the Serial Monitor will 
     not reset the board. To imitate the original behavior, the 
     developers made the Serial object return true only when the Serial 
     Monitor is detected as open, so that after Serial.begin() we can do:
     while (!Serial);
     To avoid infinite waiting, we can enhance the code like:
     while (!Serial && millis() < 5000);

  3. Then there are the ESP8266/ESP32 boards which behave differently 
     from the first two categories. They are not reset when the Serial 
     Monitor is opened, and their Serial object always returns true. 
     Moreover, even if the Serial Monitor is open while the board is 
     starting, the first printed characters are always lost if we do
     not wait for some time before printing.
    
  Given this complicated situation, we could check the board type and 
  execute architecture specific code. That would result in a long and 
  beginner-unfriendly code. My conclusion is that the best interoperable 
  solution is a delay(5000) call after Serial.begin().
  Note: 5s have been chosen, because that's the delay needed by some
        ESP8266 boards. 
*/

void setup()
{
  // Init LED
  pinMode(LED_BUILTIN, OUTPUT);

  // Init Serial (leave Serial Monitor open to see all messages)
  Serial.begin(9600); delay(5000); // wait 5s that Serial is ready
  Serial.println("Serial ready");
}

void loop()
{
  // Count-up
  static unsigned long count = 0;
  Serial.println(count++);

  // Blink
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}
