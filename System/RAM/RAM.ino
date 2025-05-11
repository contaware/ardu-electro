/*
  Show the free memory between the heap and the stack
*/
#include <FreeStack.h> // install SdFat library by Bill Greiman 
                       // https://github.com/greiman/SdFat

void setup()
{
  // Init Serial
  Serial.begin(9600);
}

void __attribute__ ((noinline)) funcCall(unsigned long param)
{
  Serial.print(F("2. Inside func: millis()=")); Serial.println(param);
  Serial.print(F("   available FREE memory: "));
  Serial.print(FreeStack()); Serial.println(F(" bytes"));
}

void loop()
{
  Serial.println(F("--------------------------------------"));
  Serial.println(F("1. Start"));
  Serial.print(F("   available FREE memory: "));
  Serial.print(FreeStack()); Serial.println(F(" bytes"));
  delay(1000);
  Serial.println(F("--------------------------------------"));
  funcCall(millis());
  delay(1000);
  Serial.println(F("--------------------------------------"));
  byte* p = (byte*)malloc(8);
  Serial.print(F("3. malloc(8): p=")); Serial.print((size_t)p); 
  Serial.print(F(" , p[0]=")); Serial.println(p[0]);
  Serial.print(F("   available FREE memory: "));
  Serial.print(FreeStack()); Serial.println(F(" bytes"));
  free(p);
}
