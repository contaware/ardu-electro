/*
  The round()/roundf() functions round a floating-point number to 
  the nearest integer. AVR is not standard because it returns an 
  integer type instead of the usually expected floating-point type.

  AVR
  do use the following macro defined in Arduino.h where long is returned:
  #define round(x)     ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
  and roundf is defined as an alias for round in <math.h>:
  #define roundf	round

  Other platforms
  do use the <math.h> functions which return floating point numbers 
  as expected (roundf() returns float and round() returns double).

  See:
  https://github.com/arduino/ArduinoCore-API/issues/76
*/
void setup()
{
  // Init Serial (leave Serial Monitor open to see all messages)
  Serial.begin(9600); delay(5000); // wait 5s that Serial is ready

  // Test float
  Serial.println("Test roundf():");
  Serial.println(roundf(1.6f));
  Serial.println(roundf(-1.6f));
  Serial.println(roundf(1.4f));
  Serial.println(roundf(-1.4f));

  Serial.println();

  // Test double
  Serial.println("Test round():");
  Serial.println(round(1.6));
  Serial.println(round(-1.6));
  Serial.println(round(1.4));
  Serial.println(round(-1.4));
}

void loop()
{

}
