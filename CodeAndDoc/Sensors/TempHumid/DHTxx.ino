/*
  DHT11 or DHT22

  - VDD supply is 3.3V - 5.5V.
  
  - The DHT22 is the more expensive version which obviously has better specifications. 
    Its temperature measuring range is from -40 to 80 °C (±0.5 accuracy), while
    the DHT11 temperature range is from 0 to 50 °C (±2 accuracy). The DHT22 has
    also a better humidity measuring range, from 0% to 100% (2% accuracy), while
    the DHT11 humidity range is from 20% to 80% (5% accuracy).
*/
#include <SimpleDHT.h>

const byte DHT_PIN = 2;

#define DHT 22                  // define 11 or 22

#if DHT == 22
SimpleDHT22 dht22(DHT_PIN);
const int POLL_RATE_MS = 2500;  // DHT22 sampling rate is 0.5 HZ 
                                // -> be above that otherwise an error is returned!
#else
SimpleDHT11 dht11(DHT_PIN);
const int POLL_RATE_MS = 1500;  // DHT11 sampling rate is 1 HZ 
                                // -> be above that otherwise an error is returned!
#endif



void setup()
{
  Serial.begin(9600);
}

void loop() 
{
// Read float
#if DHT == 22
  Serial.println("=================================");
  Serial.println("Sample DHT22...");
  float temperature = 0;
  float humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht22.read2(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess)
  {
    Serial.print("Read DHT22 failed, err="); Serial.print(SimpleDHTErrCode(err));
    Serial.print(","); Serial.println(SimpleDHTErrDuration(err)); delay(POLL_RATE_MS);
    return;
  }
// Read byte
#else
  Serial.println("=================================");
  Serial.println("Sample DHT11...");
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess)
  {
    Serial.print("Read DHT11 failed, err="); Serial.print(SimpleDHTErrCode(err));
    Serial.print(","); Serial.println(SimpleDHTErrDuration(err)); delay(POLL_RATE_MS);
    return;
  }
#endif

  // Print temperature and humidity
  Serial.print("Sample OK: ");
  Serial.print(temperature); Serial.print(" °C , ");
  Serial.print(humidity); Serial.println(" RH%");
  
  // Wait
  delay(POLL_RATE_MS);
}
