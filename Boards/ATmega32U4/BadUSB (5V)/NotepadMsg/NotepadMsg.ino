/*
  Use the board as keyboard

  - This sketch is for Windows OS: it opens the "Run" dialog,
    executes "notepad" and types "Hello World" in it.
 
  - To program the board choose Arduino Leonardo or Micro.

  - If you cannot program it anymore, try these steps:
    1. Load Blink.ino
    2. Press the reset button and do not release it.
    3. Hit the "Upload" button and when the IDE reports 
       the memory usage, release the reset button.
*/
#include "Keyboard.h"

void setup()
{               
  // Pin mode
  pinMode(LED_BUILTIN, OUTPUT);

  // Leave a delay to be able to re-program later on
  for (int i = 0 ; i < 20 ; i++)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }

  // Start the keyboard emulation
  Keyboard.begin();

  // Execute "WIN + R" which opens the "Run" dialog
  Keyboard.press(KEY_LEFT_GUI);
  delay(100);
  Keyboard.press('r');
  Keyboard.releaseAll();
  delay(500);
 
  // Run notepad.exe and type "Hello World"
  // Note: we could also use Keyboard.println("notepad")
  //       without Keyboard.write(KEY_RETURN)
  Keyboard.print("notepad");
  delay(1000);
  Keyboard.write(KEY_RETURN);
  delay(1000);
  Keyboard.print("Hello World");

  // Stop the keyboard emulation
  Keyboard.end();
}

void loop()
{
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}
