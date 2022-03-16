/*
  DHT11

  VDD supply is 3.3V - 5.5V.
*/
#include <SimpleDHT.h>

const byte DHT11_PIN = 2;

SimpleDHT11 dht11(DHT11_PIN);
const int POLL_RATE_MS = 1500; // DHT11 sampling rate is 1HZ -> be above that otherwise an error is returned!

void setup()
{
  Serial.begin(9600);
}

void loop() 
{
  // Start working
  Serial.println("=================================");
  Serial.println("Sample DHT11...");
  
  // Read with raw sample data
  byte temperature = 0;
  byte humidity = 0;
  byte data[40] = {0};
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(&temperature, &humidity, data)) != SimpleDHTErrSuccess)
  {
    Serial.print("Read DHT11 failed, err="); Serial.print(SimpleDHTErrCode(err));
    Serial.print(","); Serial.println(SimpleDHTErrDuration(err)); delay(POLL_RATE_MS);
    return;
  }

  // Print raw data
  Serial.print("Sample RAW Bits: ");
  for (int i = 0; i < 40; i++)
  {
    Serial.print((int)data[i]);
    if (i > 0 && ((i + 1) % 4) == 0)
      Serial.print(' ');
  }
  Serial.println("");

  // Print temperature and humidity
  Serial.print("Sample OK: ");
  Serial.print((int)temperature); Serial.print(" *C, ");
  Serial.print((int)humidity); Serial.println(" %");
  
  // Wait
  delay(POLL_RATE_MS);
}
