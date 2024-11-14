/* 
  Read/Write MCU's EEPROM

  - On AVR an EEPROM write takes 3.3ms to complete, read is fast.
  
  - The AVR EEPROM memory has a specified life of 100'000 write/erase 
    cycles.

  - Platforms like Arduino UNO R4 (Renesas), ESP8266 or ESP32 do emulate 
    the EEPROM library in flash memory.
*/
#include <EEPROM.h>

// Vars
unsigned long cnt;
uint16_t rnd;
  
// EEPROM.get() internally uses sizeof() to read the different type of variables
// (can also use float, double, struct or arrays)
void getEEPROM()
{
  int addr = 0;
  EEPROM.get(addr, cnt); addr += sizeof(cnt);
  Serial.print("cnt="); Serial.print(cnt); Serial.print(" , ");
  EEPROM.get(addr, rnd); addr += sizeof(rnd);
  Serial.print("rnd="); Serial.println(rnd);
}

// EEPROM.put() internally uses sizeof() to write the different type of variables
// (can also use float, double, struct or arrays)
void putEEPROM()
{
  int addr = 0;
  EEPROM.put(addr, cnt); addr += sizeof(cnt);
  EEPROM.put(addr, rnd); addr += sizeof(rnd);

  // For ESP8266 and ESP32 it's necessary to commit to flash
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
  EEPROM.commit(); // takes 30ms during which interrupts aren't handled!
#endif
}

void setup()
{ 
  // Init Serial
  Serial.begin(9600);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!
  delay(5000);      // for ESP32 and some other MCUs a delay() is needed, otherwise
                    // the messages generated in setup() can't be seen!

  Serial.println("MCU's EEPROM Test");
  
  // Begin necessary for ESP8266 and ESP32
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
  EEPROM.begin(512); // 512 should be enough for most configs
#endif

  // EEPROM size
  Serial.print("EEPROM size:  ");
  Serial.print(EEPROM.length());
  Serial.println(" bytes");

  // Get stored data
  Serial.print("Current data: ");
  getEEPROM();
  randomSeed(cnt);

  // Put new data
  ++cnt; // increasing cnt gives us a different seed with each boot
  rnd = random(0, 65536);
  putEEPROM();

  // Check the just put data
  Serial.print("New data:     ");
  getEEPROM();
}

void loop()
{

}
