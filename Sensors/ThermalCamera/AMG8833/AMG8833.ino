/*
  AMG8833 8x8 thermal camera sensor module
  
  - VDD supply is 3.3V - 5V.
  
  - The module has a 3.3V regulator and integrated level-shifters.
  
  - See the manual for the pixel disposition.
*/
#include <Adafruit_AMG88xx.h>

Adafruit_AMG88xx amg;

float pixels[AMG88xx_PIXEL_ARRAY_SIZE];

void setup()
{
  // Init Serial (leave Serial Monitor open to see all messages)
  Serial.begin(9600); delay(5000); // wait 5s that Serial is ready
  Serial.println("AMG8833 Test");

  // Default settings
  bool status = amg.begin();
  if (!status)
  {
    Serial.println("Could not find a valid AMG8833 sensor, check wiring!");
    while (true);
  }

  // Let sensor boot up
  delay(100);
}

void loop()
{ 
  // Temp
  Serial.print("Temp = ");
  Serial.print(amg.readThermistor(), 1);
  Serial.println(" °C");

  // Read all the pixels
  amg.readPixels(pixels);

  // Print
  for (int i = 1; i <= AMG88xx_PIXEL_ARRAY_SIZE; i++)
  {
    Serial.print(pixels[i-1], 0);
    Serial.print(" ");
    if (i%8 == 0)
      Serial.println();
  }
  Serial.println();
  
  delay(1000);
}
