/* 
  I2C Scanner
*/
#include <Wire.h>

void setup()
{
  // Init Serial (leave Serial Monitor open to see all messages)
  Serial.begin(9600); delay(5000); // wait 5s that Serial is ready
  Serial.println("I2C scan...");
  
  Wire.begin();

  byte devices = 0;

  /*
    Each device needs to have a unique address in the range 8 to 119. 
    Address 0 is reserved as a "broadcast" address, addresses 1 to 7 
    are reserved for other purposes, and addresses 120 to 127 are
    reserved for future use.
  */
  for (byte address = 8; address <= 119; address++)
  {
    Wire.beginTransmission(address);
    byte ret = Wire.endTransmission();
    if (ret == 0)
    {
      Serial.print("Found address: ");
      Serial.print(address, DEC);
      Serial.print(" (0x");
      Serial.print(address, HEX);
      Serial.println(")");
      devices++;
    }
    else if (ret == 4)
    {
      Serial.print("Unknown error at address: ");
      Serial.print(address, DEC);
      Serial.print(" (0x");
      Serial.print(address, HEX);
      Serial.println(")");
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
