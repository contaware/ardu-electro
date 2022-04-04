/* 
  I2C Scanner
*/
#include <Wire.h>

void setup()
{
  Serial.begin(9600);
  while (!Serial);  // if you want your program to wait for Serial Monitor to be opened before running when 
                    // using native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
  
  Serial.println("I2C scan...");
  
  Wire.begin();

  byte devices = 0;

  /*
    Each device needs to have a unique address in the range 8 to 119. 
    Address 0 is reserved as a "broadcast" address, addresses 1 to 7 
    are reserved for other purposes, and addresses 120 to 127 are
    reserved for future use. Somebody reported a sensor with address
    120, so better to scan from 1 up to 127.
  */
  for (byte address = 1; address <= 127; address++)
  {
    Wire.beginTransmission(address);
    byte ret = Wire.endTransmission();
    if (ret == 0)
    {
      Serial.print ("Found address: ");
      Serial.print (address, DEC);
      Serial.print (" (0x");
      Serial.print (address, HEX);
      Serial.println (")");
      devices++;
    }
    else if (ret == 4)
    {
      Serial.print("Unknown error at address: ");
      Serial.print (address, DEC);
      Serial.print (" (0x");
      Serial.print (address, HEX);
      Serial.println (")");
    }

    // Wait a moment
    delay(2);
  }

  Serial.print("Found ");
  Serial.print(devices, DEC);
  Serial.println(" device(s)");
}

void loop()
{
  
}
