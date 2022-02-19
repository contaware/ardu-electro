/*
  When you call myservo.write() it returns immediately (it is non-blocking) 
  and the servo will subsequently move to the new position. There's no 
  position feedback from a standard servo.
*/
#include <Servo.h>

Servo myservo; // create servo object (twelve servo objects can be created on most boards)

void setup()
{
  myservo.attach(9); // attaches the servo on pin 9 to the servo object
}

void loop()
{
  for (int pos = 0; pos <= 180; pos += 1) // goes from 0 degrees to 180 degrees
  {
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  delay(1000);
  
  for (int pos = 180; pos >= 0; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  delay(1000);
}
