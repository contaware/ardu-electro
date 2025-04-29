/*
  Serial Echo

  - It echos up to 16 chars typed into the Arduino Serial Monitor.
  
  - Because of the software serial possibility, sending and
    receiving at the same time is avoided with a 500ms delay.
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
  // Init Serial
  Serial.begin(9600);
}

void loop()
{
  // Read
  const int bufsize = 16;
  char buf[bufsize];
  int bufend = 0;
  unsigned long startMs = millis();
  while (millis() - startMs < 500)
  {
    if (Serial.available() && bufend < bufsize)
      buf[bufend++] = Serial.read();
  }

  // Write
  for (int i = 0 ; i < bufend ; i++)
    Serial.write(buf[i]);
}
