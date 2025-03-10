/*
  Free stack function from SdFat library
*/
#include <FreeStack.h> // install SdFat library by Bill Greiman 
                       // https://github.com/greiman/SdFat

void setup()
{
  // Init Serial
  Serial.begin(9600);
}

void loop()
{
  Serial.print(F("Available STACK memory: "));
  Serial.print(FreeStack()); Serial.println(F(" bytes"));
  delay(1000);
}
