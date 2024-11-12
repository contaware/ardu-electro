/*
  Hall effect sensors are used to detect a magnetic field

  The most common types are:

  1. Linear analog sensors, for example 49E.
     Those do output VDD/2 when there is no magnetic field and increase 
     up to VDD when the south pole of a magnet is facing the labeled 
     side of the Hall effect sensor and decrease down to 0V when the 
     north pole of a magnet is facing the labeled side of the Hall 
     effect sensor.
     
  2. Unipolar digital sensors, for example A1104 (replaces 3144) or US5881.
     Those turn ON with one pole and turn OFF when there is no magnetic
     field or with the other pole. These devices have built-in hysteresis.
  
  3. Latching digital sensors, for example US1881.
     Those turn ON with one pole and only turn OFF with the other pole.

  Attention: only the 49E works with a VDD supply of 3.3V - 5V. All the 
             other mentioned examples require a VDD supply of 5V - 24V.
*/

// *** This sketch shows the use of a linear analog hall sensor ***
const byte HALL_PIN = A0;

void setup()
{
  // Init Serial
  Serial.begin(9600);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!
}

void loop()
{
  Serial.println(analogRead(HALL_PIN));
  delay(300);
}
