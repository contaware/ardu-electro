/*
  Serial Print Debug Macros
*/
#define USE_DPRINT              false
#define USE_DPRINT_BETTER       true
#define DPRINT_SERIAL_SPEED     9600

// This is the better version because when compiling with USE_DPRINT set to false
// the passed parameters get verified for compilation errors. Both versions generate
// the same RAM and Flash size which proofs that also the better version correctly
// optimizes away the debug prints.
#if USE_DPRINT_BETTER == true

#define DPRINT(...)             do { if (USE_DPRINT) Serial.print(__VA_ARGS__); } while (false)
#define DPRINTLN(...)           do { if (USE_DPRINT) Serial.println(__VA_ARGS__); } while (false)
#define DWRITE(...)             do { if (USE_DPRINT) Serial.write(__VA_ARGS__); } while (false)
// Note: do-while(false) guards against if-else constructs without curly braces.

#else

#if USE_DPRINT == true
#define DPRINT(...)             Serial.print(__VA_ARGS__)
#define DPRINTLN(...)           Serial.println(__VA_ARGS__)
#define DWRITE(...)             Serial.write(__VA_ARGS__)
#else
#define DPRINT(...)
#define DPRINTLN(...)
#define DWRITE(...)
#endif

#endif

void setup()
{
  // Serial Debug
#if USE_DPRINT == true
  Serial.begin(DPRINT_SERIAL_SPEED);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!
#endif

  // Display what we are using
  DPRINTLN( USE_DPRINT_BETTER ?
            F("USE_DPRINT_BETTER= true") : // they must be the same size!
            F("USE_DPRINT_BETTER=false")); // they must be the same size!

  // Long string in RAM
  DPRINTLN("1. Lorem Ipsum is simply dummy text of the printing and typesetting industry");

  // Long string in Flash
  DPRINTLN(F("2. Lorem Ipsum is simply dummy text of the printing and typesetting industry"));

  // In case of USE_DPRINT set to false the following will only trigger an error
  // with USE_DPRINT_BETTER set to true
  //DPRINT(""Duplicated double-quotes"");
  //DPRINTLN(4XYZ); // not a number and not a string
}

void loop()
{
  
}
