/*
  Serial Echo

  - It echos up to 16 chars typed into the Arduino Serial Monitor.
  
  - Because of the software serial possibility, sending and
    receiving at the same time is avoided with a 500ms delay.
*/

// - ATTinyCore works with TX on PB0 and RX on PB1.
// - For Adafruit Trinket uncomment those two lines:
//#include <SoftwareSerial.h>
//SoftwareSerial Serial(PB1, PB0); // RX, TX
  
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
