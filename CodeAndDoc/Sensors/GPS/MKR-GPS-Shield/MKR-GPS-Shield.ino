/*
  Arduino MKR GPS Shield supporting GPS, GLONASS and Galileo
 
  - VDD supply is 3.3V (max supply current 67mA @ 1Hz).
  
  - Based on u-blox module SAM-M8Q which supports also Galileo's 
    localization option. That option is disabled by default, but it can 
    be enabled by sending the UBXCFG-GNSS configuration message.

  - Backup battery CR1216 to store configurations.

  - Communication on Serial1 at 9600 or I2C (over ESLOV connector only).
*/
#include <Arduino_MKRGPS.h>
#include <PrintCol.h>

const bool useStandby = false;
const int TIME_BETWEEN_MEASUREMENTS_MS = 10000;
const int TIME_BETWEEN_PATIENCE_MSG_MS = 60000;
unsigned long patienceMillis;

void setup()
{
  // Serial init
  Serial.begin(9600);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!
  Serial.println("Arduino MKR GPS Shield Test");

  // Begin
  // GPS_MODE_I2C to use the MKR GPS with the I2C cable (default setting).
  // GPS_MODE_SHIELD if using the MKR GPS as a shield.
  if (!GPS.begin(GPS_MODE_SHIELD))
  {
    Serial.println("Could not find the GPS module!");
    while (true);
  }

  // Print separator
  Serial.println("-----------------------------------------------------------------");

  // Reset patience counter
  patienceMillis = millis();
}

void readGPS()
{
  // Read GPS values
  float latitude   = GPS.latitude();
  float longitude  = GPS.longitude();
  float altitude   = GPS.altitude();
  float speed      = GPS.speed();
  int   satellites = GPS.satellites();

  // Print GPS values
  Serial.println("     Latitude    Longitude       Alt. m   Speed km/h     Nr. Sats");
  printCol(latitude, 7, 13);
  printCol(longitude, 7, 13);
  printCol(altitude, 1, 13);
  printCol(speed, 1, 13);
  printCol(satellites, DEC, 13);
  Serial.println();
  Serial.println("-----------------------------------------------------------------");

  // Reset patience counter
  patienceMillis = millis();
}

void emitPatienceMsg()
{
  unsigned long currentMillis = millis();
  if (currentMillis - patienceMillis > TIME_BETWEEN_PATIENCE_MSG_MS)
  { 
    Serial.println("Be patient (to get enough satellites, place the module outside)");
    patienceMillis = currentMillis; // reset patience counter
  }
}
      
void loop()
{
  if (useStandby)
  {
    Serial.println("Enter standby");
    GPS.standby();

    // Wait
    Serial.print("Delay");
    for (int i = 0; i < 10; i++)
    {
      delay(TIME_BETWEEN_MEASUREMENTS_MS / 10);
      Serial.print(".");
    }
    Serial.println();

    // Wake-up the GPS
    Serial.println("Do wake-up");
    GPS.wakeup();

    // Poll availability
    // Note: must continously poll the library, otherwise it will not work!
    while (!GPS.available())
      emitPatienceMsg();
    readGPS();
  }
  else
  {
    // The module returns a position each second
    // Note: must continously poll the library, otherwise it will not work!
    if (GPS.available())
      readGPS();
    else
      emitPatienceMsg();
  }
}
