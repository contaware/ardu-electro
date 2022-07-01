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

  - A power cycle should recall the previously stored value, but some fake
    chinese chips always start with 0, probably they do not have the NVRAM.
*/
#include <X9C10X.h> // https://github.com/RobTillaart/X9C10X
 
X9C103 pot;  // the same as X9C10X pot(10000) 
 
void setup() 
{
  Serial.begin(9600);

  // Info
  Serial.print("X9C10X_LIB_VERSION: ");
  Serial.println(X9C10X_LIB_VERSION);

  // The position parameter inits the class object with a previously stored position
  pot.begin(8, 9, 10, 0);  // pulse, direction, select, position

  // If you do not know the previously stored position, then after begin() sync. 
  // the class object with the device calling:
  pot.setPosition(0, true);
  
  // Stores the current position in device's NVRAM and returns it 
  // so that it can be stored in our code and used as position 
  // parameter the next time we boot and call begin(). As we cannot
  // read that value from the device we have to store it in device's
  // NVRAM and also in the MPU Flash/EEPROM.
  //uint8_t storedPos = pot.store();
  
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
