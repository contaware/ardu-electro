/*
  Serial Parse
  
  1. Serial.readStringUntil(terminator) will return if it encounters the given 
     terminator character. If no terminator characters is received, 
     Serial.readStringUntil(terminator) will return after the timeout set by 
     Serial.setTimeout(). If Serial.setTimeout() is not called in the code, 
     the default timeout is 1000ms.
  
  2. If the terminator character was found, Serial.readStringUntil(terminator)
     will return all characters except the terminator character. On the 
     contrary, if Serial.readStringUntil(terminator) timed-out, all received 
     character are returned. 
  
  3. When Serial.readStringUntil(terminator) returns, all characters are 
     discarded from the serial buffer including the terminator character if 
     one was sent.

  Note: the timeout resets as soon as a character is received. Therefore, 
        the sender can wait up to the configured timeout between sent 
        characters. To verify this, use a serial terminal that sends the 
        characters as you type them. Remember that the Arduino Serial Monitor 
        sends all characters at once when you press ENTER.
*/

void doSerialRead()
{
  String msg;
  msg = Serial.readStringUntil('\n'); // function removes '\n' from serial buffer and does not return a '\n'
  msg.trim();                         // remove CR if terminal is sending one
  if (msg.length() == 0)              // if just pressing ENTER
    Serial.println("Just pressed ENTER");
  else
  {
    switch (toupper(msg[0]))
    {
      case 'A':
        msg.remove(0, 1);               // remove 'A' char
        msg.trim();                     // trim whitespaces
        Serial.print("Command A followed by: ");
        Serial.println(msg);
        break;

      case 'B':
        msg.remove(0, 1);               // remove 'B' char
        msg.trim();                     // trim whitespaces
        Serial.print("Command B followed by: ");
        Serial.println(msg);
        break;

      default:
        Serial.print("Unknown command: ");
        Serial.println(msg);
        break;
    }
  }
}

void setup()
{
  // Init Serial (leave Serial Monitor open to see all messages)
  Serial.begin(9600); delay(5000); // wait 5s that Serial is ready
  Serial.println("Type in upper window and press ENTER:");

  // Set timeout to 5s
  Serial.setTimeout(5000); // defaults to 1s
}

void loop()
{
  if (Serial.available())
    doSerialRead();
}
