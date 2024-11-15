/*
  X9C10X digital potentiometer
 
        +--------+
  INC 1 | o    o | 8 VCC
  U/D 2 | o    o | 7 CS
  RH  3 | o    o | 6 RL
  GND 4 | o    o | 5 Rwiper
        +--------+

  - INC   pulses
    U/D   UP = 1 DOWN = 0
    VCC   +5V
    CS    to control multiple devices
  
  - X9C102(uint32_t Ohm = 1000)
    X9C103(uint32_t Ohm = 10000)
    X9C104(uint32_t Ohm = 100000)
    X9C503(uint32_t Ohm = 50000)

  - A power cycle recalls the previously stored position. Unfortunately 
    we cannot read that value from the device. For this reason it only 
    makes sense to use store() with the X9C base class that has no 
    internal position variable.
    ATTENTION: use 10kΩ pull-ups on INC, U/D and CS lines to avoid 
               unwanted STORE pulses.
*/
#include <X9C10X.h> // https://github.com/RobTillaart/X9C10X

// This object starts with the internal _position variable set to 0
X9C103 pot;  // the same as X9C10X pot(10000)

const byte PULSE_PIN = 8;
const byte DIRECTION_PIN = 9;
const byte SELECT_PIN = 10;

void setup() 
{
  // Init Serial
  Serial.begin(9600);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!
  delay(5000);      // for ESP32 and some other MCUs a delay() is needed, otherwise
                    // the messages generated in setup() can't be seen!

  // Info
  Serial.print("X9C10X_LIB_VERSION: ");
  Serial.println(X9C10X_LIB_VERSION);

  // Init pins
  pot.begin(PULSE_PIN, DIRECTION_PIN, SELECT_PIN);

  // Set initial position
  // Note: if we know to which position the device restores we can 
  //       use pot.restoreInternalPosition(pos) instead or call no
  //       function at all if it restores to the class default of 0.
  pot.setPosition(0, true); // ATTENTION: this steps through all values!
  
  // Return the maximum value (=parameter from constructor)
  uint32_t potSize = pot.getMaxOhm();
  Serial.print("Pot size = ");
  Serial.print(potSize);
  Serial.println(" Ω");

  // Print start pos
  printPos();
  delay(2000);
}

void printPos()
{
  uint8_t pos = pot.getPosition();
  uint32_t ohm = pot.getOhm(); // the returned value does depend on the value passed in the constructor
  Serial.print("Pot pos = ");
  Serial.print(pos);
  Serial.print(" (");
  Serial.print(ohm);
  Serial.println(" Ω)");
}

void loop() 
{
  // Go up
  for (int i = 0; i < 99; i++)
  { 
    pot.incr(); // moves one position up (if possible)
    printPos();
    delay(2000);
  }

  // Go down
  for (int i = 99; i > 0; i--)
  {
    pot.decr(); // moves one position down (if possible)
    printPos();
    delay(2000);
  }
}
