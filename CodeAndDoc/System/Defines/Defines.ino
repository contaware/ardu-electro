/*
  Check the architecture #define for your used board
*/

void setup() 
{
  Serial.begin(9600);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!

  // Init message
  Serial.println(F("Architecture define(s):"));
  
  // All AVRs: traditional + megaAVR
  #if defined(__AVR__)
    Serial.println(F("__AVR__"));
  #endif

  // Traditional AVRs (UNO, Nano, Mega, Leonardo, ATtiny, ...) 
  #if defined(ARDUINO_ARCH_AVR)
    Serial.println(F("ARDUINO_ARCH_AVR"));
  #endif

  // ATtiny85
  #if defined(__AVR_ATtiny85__)
    Serial.println(F("__AVR_ATtiny85__"));
  #endif

  // megaAVRs (UNO WiFi rev2 and Nano Every)
  #if defined(ARDUINO_ARCH_MEGAAVR)
    Serial.println(F("ARDUINO_ARCH_MEGAAVR"));
  #endif

  // Samd21 (Zero, MKR, Nano 33)
  #if defined(ARDUINO_ARCH_SAMD)
    Serial.println(F("ARDUINO_ARCH_SAMD"));
  #endif

  // Sam (Due)
  #if defined(ARDUINO_ARCH_SAM)
    Serial.println(F("ARDUINO_ARCH_SAM"));
  #endif

  // ESP8266
  #if defined(ARDUINO_ARCH_ESP8266)
    Serial.println(F("ARDUINO_ARCH_ESP8266"));
  #endif

  // ESP32
  #if defined(ARDUINO_ARCH_ESP32)
    Serial.println(F("ARDUINO_ARCH_ESP32"));
  #endif

  // STM32
  #if defined(ARDUINO_ARCH_STM32)
    Serial.println(F("ARDUINO_ARCH_STM32"));
  #endif

  // RP2040
  #if defined(ARDUINO_ARCH_RP2040)
    Serial.println(F("ARDUINO_ARCH_RP2040"));
  #endif

  // UNO R4 (Minima and WiFi)
  #if defined(ARDUINO_UNOR4_MINIMA)
    Serial.println(F("ARDUINO_UNOR4_MINIMA"));
  #endif
  #if defined(ARDUINO_UNOR4_WIFI)
    Serial.println(F("ARDUINO_UNOR4_WIFI"));
  #endif
}

void loop()
{

}
