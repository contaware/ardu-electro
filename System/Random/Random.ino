/* 
  Generate a better seed than randomSeed(analogRead(A0))

  - The problem of analogRead(A0) is that the returned values are 
    usually clustered between two values and thus not distribuited over 
    the entire ADC range.

  - We only use the LSB of the ADC which is more noisy and better 
    distribuited compared to the other bits.

  - The random() function of ESP8266 & ESP32 by default uses a real 
    hardware random number generator. But when randomSeed() is called, 
    it turns to a psedo-random generator, so do not call randomSeed() 
    for ESP8266 & ESP32!
    https://github.com/espressif/arduino-esp32/pull/7848
*/
#include "PrintCol.h"

#define UNCONNECTED_ANALOG_PIN   A0
 
unsigned long bins[32] = {0};
unsigned long lastMillis;

unsigned long genSeed()
{
  unsigned long seed = 0;
  for (int i = 0; i < 32; i++)
  {
    seed |= (unsigned long)(analogRead(UNCONNECTED_ANALOG_PIN) & 1) << i;
    delay(2); // let the pin fluctuate
  }
  return seed;
}

void setup()
{
  // Init Serial (leave Serial Monitor open to see all messages)
  Serial.begin(9600); delay(5000); // wait 5s that Serial is ready

  // In your sketch you would just do the following
  unsigned long seed = genSeed();
  randomSeed(seed);
  Serial.print("Seed="); Serial.println(seed);
  Serial.print("Random1[0..255]="); Serial.println(random(0, 256));
  Serial.print("Random2[0..255]="); Serial.println(random(0, 256));
  Serial.print("Random3[0..255]="); Serial.println(random(0, 256));
  
  // Init variable used to serial print a table with the statistics
  lastMillis = millis();  
}

void loop()
{
  // This code is to display the distribution of the seeds over the unsigned long 
  // number space. We divide it in 32 bins: 2^32 / 32 = 134217728
  // Run that for a day or so and check whether it is uniformly distributed.
  ++bins[genSeed() / 134217728UL];
  unsigned long currentMillis = millis();
  if ((currentMillis - lastMillis) > 60000)
  {
    lastMillis = currentMillis;
    for (int i = 0; i < 32; i++)
      printCol(bins[i], DEC, 6);
    Serial.println();
  }
}
