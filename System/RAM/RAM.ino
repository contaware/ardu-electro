/*
  Show the free memory
  
  - For AVR and ARMs without an embedded OS, the FreeStack() function 
    returns the free memory between the heap and the stack. The free 
    stack is the same as the free heap.

  - ESP8266 has 2 stacks, the sys stack and the cont stack. The sys 
    is used by the system, while the cont is the one used by setup(), 
    loop() and all functions called from those two functions. The size 
    of the cont stack is 4KB.

  - ESP32 dynamically allocates stacks from the heap on tasks creation.
    The task which runs setup(), loop() and all functions called 
    from those two functions has a stack of 8KB.

  - MBED dynamically allocates stacks from the heap on threads creation.
    The thread which runs setup(), loop() and all functions called 
    from those two functions has a stack of 32KB.
*/
#if defined(ARDUINO_ARCH_MBED)
#include "mbed.h"
#elif !defined(ARDUINO_ARCH_ESP8266) && !defined(ARDUINO_ARCH_ESP32)
#include <FreeStack.h> // install SdFat library by Bill Greiman 
                       // https://github.com/greiman/SdFat
#endif

// For malloc(), use a global volatile variable 
// to prevent compiler optimizations
volatile byte* p = nullptr;

void __attribute__((always_inline)) inline printRAM()
{
#if defined(ARDUINO_ARCH_ESP8266)
  Serial.print(F("   FREE HEAP:   "));
  Serial.print(ESP.getFreeHeap());
#elif defined(ARDUINO_ARCH_ESP32)
  Serial.print(F("   FREE HEAP:   "));
  Serial.print(esp_get_free_heap_size());
#elif defined(ARDUINO_ARCH_MBED)
  mbed_stats_heap_t heap_stats;
  mbed_stats_heap_get(&heap_stats);
  Serial.print(F("   HEAP usage:  "));
  Serial.print(heap_stats.current_size); 
  Serial.print(F("/")); 
  Serial.print(heap_stats.reserved_size);
#else
  Serial.print(F("   FREE memory: "));
  Serial.print(FreeStack());
#endif
  Serial.println(F(" bytes"));
}

void __attribute__((noinline)) funcCall(unsigned long param)
{
  Serial.print(F("2. Inside func: millis()=")); Serial.println(param);
  printRAM();
}

void setup()
{
  // Init Serial (leave Serial Monitor open to see all messages)
  Serial.begin(9600); delay(5000); // wait 5s that Serial is ready
  
  Serial.println();
  Serial.println(F("--------------------------------------------------------------"));
  Serial.println(F("1. Start"));
  printRAM();
  Serial.println(F("--------------------------------------------------------------"));
  funcCall(millis());
  Serial.println(F("--------------------------------------------------------------"));
  p = (volatile byte*)malloc(256);
  Serial.print(F("3. malloc(256): p=0x")); Serial.print((size_t)p, HEX);
  Serial.print(F(" , p[0]=")); Serial.print(p[0]);
  Serial.print(F(" , p[255]=")); Serial.println(p[255]);
  printRAM();
  free((void*)p);
  Serial.println(F("--------------------------------------------------------------"));
}

void loop()
{

}
