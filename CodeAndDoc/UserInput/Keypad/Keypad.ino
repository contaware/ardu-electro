/*
  Poll a Keypad

  - The library pulls each of the columns LOW one at a time, and then 
    reads the state of the rows one at a time (the row pins are set to 
    INPUT_PULLUP mode).

  - The library debounces the keys for us.

  - The library supports multiple keypresses.
*/
#include <Keypad.h>  // by Mark Stanley, Alexander Brevig

const byte ROWS = 4; // four rows
const byte COLS = 4; // four columns
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6}; // connect these digital pins to the row pins of the keypad
byte colPins[COLS] = {5, 4, 3, 2}; // connect these digital pins to the column pins of the keypad

// Initialize an instance of class Keypad
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void setup()
{
  // Init Serial
  Serial.begin(9600);
}
  
void loop()
{
  // Poll a single keypress
  char customKey = customKeypad.getKey();
  if (customKey)
    Serial.println(customKey);
}
