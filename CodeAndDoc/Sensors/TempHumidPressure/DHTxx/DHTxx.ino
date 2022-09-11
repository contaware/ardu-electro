/*
  DHT11/DHT22 humidity and temperature sensor

  - VDD supply is 3.3V - 5.5V.
  
  - The DHT22 is the more expensive version which obviously has better specifications. 
    Its temperature measuring range is from -40 to 80 °C (±0.5 accuracy), while
    the DHT11 temperature range is from 0 to 50 °C (±2 accuracy). The DHT22 has
    also a better humidity measuring range, from 0% to 100% (2% accuracy), while
    the DHT11 humidity range is from 20% to 80% (5% accuracy).
*/
#include <SimpleDHT.h>
#include <PrintCol.h>

const byte DHT_PIN = 2;                         // set the signal PIN
#define DHT 22                                  // choose type: 11 or 22

#if DHT == 22
SimpleDHT22 dht22(DHT_PIN);
const int TIME_BETWEEN_MEASUREMENTS_MS = 2500;  // DHT22 sampling rate is 0.5 HZ 
                                                // -> be above that otherwise an error is returned!
#else
SimpleDHT11 dht11(DHT_PIN);
const int TIME_BETWEEN_MEASUREMENTS_MS = 1500;  // DHT11 sampling rate is 1 HZ 
                                                // -> be above that otherwise an error is returned!
#endif

void setup()
{
  Serial.begin(9600);

#if DHT == 22
  Serial.println("Read DHT22");
#else
  Serial.println("Read DHT11");
#endif
}

void loop() 
{
  Serial.println("----------------------");
  
// Read float
#if DHT == 22
  float temperature = 0;
  float humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht22.read2(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess)
  {
    Serial.print("DHT22 error="); Serial.print(SimpleDHTErrCode(err));
    Serial.print(","); Serial.println(SimpleDHTErrDuration(err)); delay(TIME_BETWEEN_MEASUREMENTS_MS);
    return;
  }
// Read byte
#else
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess)
  {
    Serial.print("DHT11 error="); Serial.print(SimpleDHTErrCode(err));
    Serial.print(","); Serial.println(SimpleDHTErrDuration(err)); delay(TIME_BETWEEN_MEASUREMENTS_MS);
    return;
  }
#endif

  // Print humidity and temperature
  Serial.println(" Humidity %   Temp. °C");
  printCol(humidity);
  printCol(temperature);
  Serial.println();
  
  // Wait
  delay(TIME_BETWEEN_MEASUREMENTS_MS);
}
