/*
  RobotCar

  - The popular RobotCar is available with 2 or 4 wheels.
    This sketch works for both types.

  - Here we can see how a project is divided into multiple files.

  - Under RC-Config.h configure your RobotCar(s).
*/
#include "RC-Config.h"
#include "RC-Motor.h"
#include "RC-IR.h"
#include "RC-Servo.h"
#include "RC-US.h"
#include "RC-PE.h"
#include "RC-LCD.h"

void setup()
{ 
  // Serial Debug
#if USE_DPRINT == true
  Serial.begin(DPRINT_SERIAL_SPEED);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!
  DPRINT(F("RobotCar from ")); DPRINT(F(__DATE__)); DPRINT(F(", ")); DPRINTLN(F(__TIME__));
#endif

  // 16x2 LCD Display over I2C
#if USE_LCD == true
  lcdBegin();
#endif

  // DC Motor (L298 driver)
  motorBegin();

  // Servo
#if USE_SERVO == true
  g_servo.attach(SERVO_PIN,
                 700,     // the pulse width, in microseconds, corresponding to the minimum (0-degree) angle on the servo
                 2400);   // the pulse width, in microseconds, corresponding to the maximum (180-degree) angle on the servo
  g_servo.write(90);
#endif

  // Ultrasonic sensor (HC-SR04)
#if USE_ULTRASONIC_SENSOR == true
  ultrasonicBegin();
#endif

  // Photo electric encoder (HC-020K)
#if USE_PHOTO_ENC == true
  photoEncBegin();
#endif

  // IR (KY-022)
  irBegin();
}

void loop()
{
  irControl();

#if USE_ULTRASONIC_SENSOR == true
  g_ultrasonicTimer.process();
#endif

#if USE_PHOTO_ENC == true
  g_photoEncTimer.process();
#endif
}
