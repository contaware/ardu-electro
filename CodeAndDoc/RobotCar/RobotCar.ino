#include "RC-Config.h"
#include "RC-Motor.h"
#include "RC-IR.h"
#include "RC-BT.h"
#include "RC-PS2X.h"
#include "RC-Servo.h"
#include "RC-US.h"
#include "RC-PE.h"
#include "RC-LT.h"
#include "RC-LCD.h"
#include <EEPROM.h>

// Mode
// 0: Motors OFF, 1: Motors IR, 2: Motors BT, 3: Motors PS2, 4: Line tracking, 5: Obstacle avoidance
int g_mode = 0;
void setMode(int mode)
{
  // Set new mode
  if (mode < 0)
    mode = 5;
  else if (mode > 5)
    mode = 0;
  g_mode = mode;

  // Stop motors
  motorSpeed(0);

  // Reset possible running auto movement
#if USE_MOTOR_AUTO == 1
  motorAutoReset();
#endif

  // Update display
#if USE_LCD == 1
  displayMode();
#endif
}

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
  
  // Serial
#if DEBUG == true || USE_BLUETOOTH == 1
  Serial.begin(SERIAL_SPEED);
  while (!Serial);  // if you want your program to wait for Serial Monitor to be opened before running when 
                    // using native USB boards (e.g., Leonardo, Micro, MKR, Nano 33 IoT)
#endif
  
  // DC Motor (L298)
  motorBegin();

  // 16x2 LCD Display over I2C
#if USE_LCD == 1
  g_lcd.init();
  g_lcd.backlight();      // turn on the backlight
  g_lcd.clear();          // returns to home position and clears everything, while home() just returns to home position
  displayMode();
#endif

  // Servo
#if USE_SERVO == 1
  g_servo.attach(SERVO_PIN,
                 700,     // the pulse width, in microseconds, corresponding to the minimum (0-degree) angle on the servo
                 2400);   // the pulse width, in microseconds, corresponding to the maximum (180-degree) angle on the servo
  g_servo.write(90);
#endif

  // PS2 controller
#if USE_PS2_CONTROLLER == 1
  delay(500);  // added delay to give wireless ps2 module some time to startup, before configuring it
  g_ps2x.config_gamepad(PS2_CLK_PIN, PS2_CMD_PIN, PS2_SEL_PIN, PS2_DAT_PIN);
  g_ps2xTimer.begin(70, ps2Control);
  delay(500);  // added delay to give wireless ps2 module some time to finish config
#endif

  // Ultrasonic sensor (HC-SR04)
#if USE_ULTRASONIC_SENSOR == 1
  ultrasonicBegin();
#endif

  // Photo electric encoder (HC-020K)
#if USE_PHOTO_ENC == 1
  photoEncBegin();
#endif

  // Line tracking (KY-033)
#if USE_LINE_TRACKING == 1
  lineTrackingBegin();
#endif

  // IR (KY-022)
#if USE_IR_RECEIVER == 1
  irBegin();
#endif

  // Bluetooth (ZS-040)
#if USE_BLUETOOTH == 1
  bluetoothBegin();
#endif
}

void loop()
{
#if USE_IR_RECEIVER == 1
  irControl();
#endif

#if USE_BLUETOOTH == 1
  bluetoothControl();
#endif

#if USE_PS2_CONTROLLER == 1
  g_ps2xTimer.process();
#endif

#if USE_ULTRASONIC_SENSOR == 1
  g_ultrasonicTimer.process();
#endif

#if USE_PHOTO_ENC == 1
  g_photoEncTimer.process();
#endif

  if (g_mode == 4) // line tracking mode
  {
#if USE_LINE_TRACKING == 1
    g_lineTrackingTimer.process();
#endif
  }
  else if (g_mode == 5)  // obstacle avoidance mode
  {
#if USE_MOTOR_AUTO == 1
    motorAutoProcess();
#endif
  }
}
