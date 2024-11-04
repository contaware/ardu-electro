#include <PS2Keyboard.h>  // by PaulStoffregen  https://www.pjrc.com/teensy/td_libs_PS2Keyboard.html
                          //                    https://github.com/PaulStoffregen/PS2Keyboard

const byte DATA_PIN = 8;  // Data
const byte IRQ_PIN =  3;  // CLK

PS2Keyboard keyboard;

void setup()
{
  delay(1000);
  keyboard.begin(DATA_PIN, IRQ_PIN, PS2Keymap_German); // PS2Keymap_US, PS2Keymap_German, PS2Keymap_French, PS2Keymap_Spanish, PS2Keymap_Italian, PS2Keymap_UK
  Serial.begin(9600);
  Serial.println("Keyboard Test:");
}

void loop()
{
  if (keyboard.available()) // returns true if at least one keystroke available
  {  
    // Read the next key
    char c = keyboard.read(); // -1 is returned if no keystrokes have been received (use a int)
    
    // Check for some of the special keys
    if (c == PS2_ENTER)
      Serial.println();
    else if (c == PS2_TAB)
      Serial.print("[Tab]");
    else if (c == PS2_ESC)
      Serial.print("[ESC]");
    else if (c == PS2_PAGEDOWN)
      Serial.print("[PgDn]");
    else if (c == PS2_PAGEUP)
      Serial.print("[PgUp]");
    else if (c == PS2_LEFTARROW)
      Serial.print("[Left]");
    else if (c == PS2_RIGHTARROW)
      Serial.print("[Right]");
    else if (c == PS2_UPARROW)
      Serial.print("[Up]");
    else if (c == PS2_DOWNARROW)
      Serial.print("[Down]");
    else if (c == PS2_DELETE)
      Serial.print("[Del or Backspace]");
    else {
      // Otherwise just print all normal characters
      Serial.print(c);
    }
  }
}
