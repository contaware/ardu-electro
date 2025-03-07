/*
  Check the architecture #define for your used board
*/

// For boards without Serial set to 1, otherwise 0
#define USE_SOFTWARE_SERIAL   0

#if USE_SOFTWARE_SERIAL == 1
#include <SoftwareSerial.h>
#define RX_PIN                3
#define TX_PIN                4
SoftwareSerial Serial(RX_PIN, TX_PIN);
#endif

void setup() 
{
  // Init Serial (leave Serial Monitor open to see all messages)
  Serial.begin(9600); delay(5000); // wait 5s that Serial is ready
  Serial.print(F("Compiled: ")); Serial.print(F(__DATE__)); Serial.print(F(", ")); Serial.println(F(__TIME__));
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

  // All ARMs
  #if defined(__arm__)
    Serial.println(F("__arm__"));
  #endif

  // All Samd: Samd21 + Samd51
  #if defined(ARDUINO_ARCH_SAMD)
    Serial.println(F("ARDUINO_ARCH_SAMD"));
  #endif

  // Samd21 (Zero, MKR, Nano 33 IoT)
  #if defined(ARDUINO_ARCH_SAMD) && !defined(__SAMD51__)
    Serial.println(F("SAMD21"));
  #endif
  
  // Samd51 (Adafruit Metro M4, ...)
  #if defined(__SAMD51__)
    Serial.println(F("__SAMD51__"));
  #endif

  // Renesas (UNO R4 Minima/WiFi and Portenta C33)
  #if defined(ARDUINO_ARCH_RENESAS)
    Serial.println(F("ARDUINO_ARCH_RENESAS"));
  #endif
  #if defined(ARDUINO_UNOR4_MINIMA)
    Serial.println(F("ARDUINO_UNOR4_MINIMA"));
  #endif
  #if defined(ARDUINO_UNOR4_WIFI)
    Serial.println(F("ARDUINO_UNOR4_WIFI"));
  #endif
  #if defined(ARDUINO_PORTENTA_C33)
    Serial.println(F("ARDUINO_PORTENTA_C33"));
  #endif
  
  // Sam (Due)
  #if defined(ARDUINO_ARCH_SAM)
    Serial.println(F("ARDUINO_ARCH_SAM"));
  #endif

  // Mbed (Nano 33 BLE, Nano RP2040 Connect, Nicla and Portenta H7/X8)
  #if defined(ARDUINO_ARCH_MBED)
    Serial.println(F("ARDUINO_ARCH_MBED"));
  #endif
  #if defined(ARDUINO_ARCH_NRF52840)
    Serial.println(F("ARDUINO_ARCH_NRF52840"));
  #endif
  #if defined(ARDUINO_ARCH_RP2040)
    Serial.println(F("ARDUINO_ARCH_RP2040"));
  #endif

  // STM32
  #if defined(ARDUINO_ARCH_STM32)
    Serial.println(F("ARDUINO_ARCH_STM32"));
  #endif

  // ESP8266
  #if defined(ARDUINO_ARCH_ESP8266)
    Serial.println(F("ARDUINO_ARCH_ESP8266"));
  #endif

  // ESP32
  #if defined(ARDUINO_ARCH_ESP32)
    Serial.println(F("ARDUINO_ARCH_ESP32"));
  #endif
}

void loop()
{

}
