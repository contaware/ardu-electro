/*
  Print the default I2S pins for the selected board
*/

void setup()
{
  // Init serial
  Serial.begin(9600);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!
  delay(5000);      // for ESP32 and some other MCUs a delay() is needed, otherwise
                    // the messages generated in setup() can't be seen!

  Serial.println("Default I2S pins:");

  #if defined(PIN_I2S_FS)
    Serial.print("PIN_I2S_FS="); Serial.println(PIN_I2S_FS);
  #else
    Serial.println("PIN_I2S_FS=NOT DEFINED");
  #endif

  #if defined(PIN_I2S_SCK)
    Serial.print("PIN_I2S_SCK="); Serial.println(PIN_I2S_SCK);
  #else
    Serial.println("PIN_I2S_SCK=NOT DEFINED");
  #endif
  
  #if defined(PIN_I2S_SD)
    Serial.print("PIN_I2S_SD="); Serial.println(PIN_I2S_SD);
  #else
    Serial.println("PIN_I2S_SD=NOT DEFINED");
  #endif

  #if defined(PIN_I2S_SDI) && defined(PIN_I2S_SDO)        // SAMD51
    Serial.print("PIN_I2S_SDO="); Serial.println(PIN_I2S_SDO);
    Serial.print("PIN_I2S_SDI="); Serial.println(PIN_I2S_SDI);
  #endif
  
  #if defined(PIN_I2S_SD_IN) && defined(PIN_I2S_SD_OUT)   // Nano ESP32
    Serial.print("PIN_I2S_SD_OUT="); Serial.println(PIN_I2S_SD_OUT);
    Serial.print("PIN_I2S_SD_IN="); Serial.println(PIN_I2S_SD_IN);
  #endif
}

void loop()
{
  
}
