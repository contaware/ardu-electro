/* 
  Drive a BLDC (Brushless DC motor) with an ESC (Electronic Speed Controller)

  - An ESC has always three control wires, in the middle there is the 5V output (red), 
    never wire that one when connecting the Arduino to the USB port or if powering 
    the Arduino from another source! Connect GND (black or brown) to the Arduino GND 
    and the signal wire to the Arduino ESC_PIN defined below.
  
  - Most ESCs must be armed before being used. The arming procedure depends from
    the ESC model. Bidirectional ESCs are usually armed in middle position (90°),
    while monodirectional ESCs are armed by setting the position to 0°.
*/
#include <Servo.h>

#define ESC_PIN       9
#define POT_PIN       A0

Servo ESC; // create servo object (twelve servo objects can be created on most boards)
unsigned long lastMillis;

void setup()
{
  // Init Serial
  Serial.begin(9600);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!
  delay(5000);      // for ESP32 and some other MCUs a delay() is needed, otherwise
                    // the messages generated in setup() can't be seen!

  Serial.println("Drive a BLDC with an ESC");

  // Init min and max pulse width in us, each ESC is a bit different, test it!
  ESC.attach(ESC_PIN,
            1000,   // 0°, default is MIN_PULSE_WIDTH=544us
            2000);  // 180°, default is MAX_PULSE_WIDTH=2400us

  // Init millis var
  lastMillis = millis();
}

void loop()
{
  // Regulate
  int potVal = analogRead(POT_PIN);
  int val = map(potVal, 0, 1023, 0, 180);
  ESC.write(val);

  // Serial Print
  unsigned long currentMillis = millis();
  if (currentMillis - lastMillis > 100)
  {
    lastMillis = currentMillis;
    Serial.print(ESC.read());
    Serial.print("°=");
    Serial.print(ESC.readMicroseconds());
    Serial.println("us");
  }
}
