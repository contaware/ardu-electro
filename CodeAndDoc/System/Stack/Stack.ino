/*
  Free stack function from SdFat library
*/
#include <FreeStack.h> // from https://github.com/greiman/SdFat

void setup()
{
  // Serial Debug
  Serial.begin(9600);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!
}

void loop()
{
  Serial.print(F("Available STACK memory: "));
  Serial.print(FreeStack()); Serial.println(F(" bytes"));
  delay(1000);
}
