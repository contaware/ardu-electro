#include "RC-Config.h"
#include "RC-Motor.h"
#include "RC-IR.h"
#include "RC-Servo.h"
#include "RC-US.h"
#include "RC-PE.h"
#include "RC-LCD.h"
#include <EEPROM.h>

void setup()
{
  // Random seed
  // For true randomness there is the complicated entropy library,
  // but to just have a different seed with each boot it's good 
  // enough to read a counter from EEPROM (this is also better than
  // using analogRead() of an unconnected pin)
  const int seed_addr = 0;
  unsigned long seed_count;
  EEPROM.get(seed_addr, seed_count);
  EEPROM.put(seed_addr, seed_count + 1);
  randomSeed(seed_count);
  
  // Serial Debug
#if USE_DPRINT == true
  Serial.begin(DPRINT_SERIAL_SPEED);
  while (!Serial);  // for native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
                    // that waits here until the user opens the Serial Monitor!
#endif
  
  // DC Motor (L298)
  motorBegin();

  // 16x2 LCD Display over I2C
#if USE_LCD == true
  g_lcd.init();
  g_lcd.backlight();      // turn on the backlight
  g_lcd.createChar(0, arrow_left_char);
  g_lcd.createChar(1, arrow_right_char);
  g_lcd.createChar(2, arrow_up_char);
  g_lcd.createChar(3, arrow_down_char);
  g_lcd.clear();          // returns to home position and clears everything, while home() just returns to home position
  displayMotorState();
#endif

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
