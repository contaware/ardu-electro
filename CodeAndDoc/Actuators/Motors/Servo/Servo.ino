/*
  Control a Servo with an Arduino

  - Servos have always three wires, the middle wire is usually red and it has 
    to be supplied with 5V (for bigger servos an external power source may be 
    needed). The brown or black wire is the GND and the PWM signal wire can be 
    orange, yellow or white.
  
  - The PWM signal is a square wave of 20ms period (50Hz) with the HIGH having 
    a voltage of 5V and the LOW being GND. The pulse width determines the 
    position, usually it is between 1-2ms: 
    * 1ms represents the 0° position.
    * 1.5ms represents the 90° position. 
    * 2ms represents the 180° position.
  
  - Internally a servo is made of a DC motor with reduction gears. There is a 
    feedback circuitry consisting of a pulse width comparator, which compares the 
    PWM signal with a one-shot timer whose period depends on the resistance of a 
    potentiometer connected to the servo's drive shaft.
  
  - On AVR boards other than the Mega, the servo library uses Timer1 for itself. 
    This means that analogWrite() will not work on pins 9 and 10.

  - The myservo.write() function returns immediately (it is non-blocking) and the 
    servo will subsequently move to the new position. It's not possible to know 
    when the servo reaches the wanted position.
*/
#include <Servo.h>

#define SERVO_PIN     9
#define POT_PIN       A0

Servo myservo; // create servo object (twelve servo objects can be created on most boards)
unsigned long lastMillis;

void setup()
{
  // Init Serial
  Serial.begin(9600);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!
  delay(5000);      // for ESP32 and some other MCUs a delay() is needed, otherwise
                    // the messages generated in setup() can't be seen!

  Serial.println("Servo Test");

  // Init min and max pulse width in us, each Servo is a bit different, test it!
  myservo.attach(SERVO_PIN,
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
  myservo.write(val);

  // Serial Print
  unsigned long currentMillis = millis();
  if (currentMillis - lastMillis > 100)
  {
    lastMillis = currentMillis;
    Serial.print(myservo.read());
    Serial.print("°=");
    Serial.print(myservo.readMicroseconds());
    Serial.println("us");
  }
}
