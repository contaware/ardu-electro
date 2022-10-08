/*  
  Configuration file
*/ 
#ifndef rc_config_h
#define rc_config_h

#include <Arduino.h>


// Serial Debug
// - if USE_DPRINT is set to true, DPRINT, DPRINTLN and DWRITE do output to Serial Monitor.
// - if USE_DPRINT is set to false, DPRINT, DPRINTLN and DWRITE are optimized away.
#define USE_DPRINT                true
#define DPRINT_SERIAL_SPEED       115200
#define DPRINT(...)               do { if (USE_DPRINT) Serial.print(__VA_ARGS__); } while (false)
#define DPRINTLN(...)             do { if (USE_DPRINT) Serial.println(__VA_ARGS__); } while (false)
#define DWRITE(...)               do { if (USE_DPRINT) Serial.write(__VA_ARGS__); } while (false)
// Note: do-while(false) guards against if-else constructs without curly braces.


// Mode
extern int g_mode;
extern void setMode(int mode);


// Robot Car Model
#define ROBOTCAR_MODEL            1     // 1: Lafvin, 2: Elegoo


// Lafvin
#if ROBOTCAR_MODEL == 1               

// 1: use it, 0: do not use it
#define USE_LCD                   true
#define USE_IR_RECEIVER           true
#define USE_BLUETOOTH             true
#define USE_PS2_CONTROLLER        false
#define USE_LINE_TRACKING         false
#define USE_MOTOR_AUTO            true  // needs: USE_SERVO + USE_ULTRASONIC_SENSOR / USE_VL53L0X_SENSOR + USE_PHOTO_ENC
#define USE_SERVO                 true
#define USE_ULTRASONIC_SENSOR     false // attention: cannot enable both USE_ULTRASONIC_SENSOR and USE_VL53L0X_SENSOR
#define USE_VL53L0X_SENSOR        true  // attention: cannot enable both USE_ULTRASONIC_SENSOR and USE_VL53L0X_SENSOR
#define USE_PHOTO_ENC             true

// IR Receiver
#define IR_RECEIVE_PIN            49    // module: G=GND, R=5V, Y=Signal ; receiver front: 1=Signal, 2=GND, 3=5V

// Bluetooth
#define BLUETOOTH_SERIAL          Serial1 // Serial, Serial1, Serial2 or Serial3
#define BLUETOOTH_SERIAL_SPEED    9600

// PS2 controller
#define PS2_DAT_PIN               A0
#define PS2_CMD_PIN               A1
#define PS2_SEL_PIN               4
#define PS2_CLK_PIN               13

// Servo
#define SERVO_PIN                 10    // on boards other than the Mega, use of the library disables analogWrite() (PWM)
                                        // functionality on pins 9 and 10, whether or not there is a Servo on those pins.

// Ultrasonic sensor (HC-SR04)
#define ULTRASONIC_ECHO_PIN       A2
#define ULTRASONIC_TRIG_PIN       A3

// Photo electric encoder (HC-020K)
#define PHOTO_ENC_RIGHT_PIN       2     // needs hardware interrupts: UNO pin 2, 3 | MEGA pin 2, 3, 18, 19
#define PHOTO_ENC_LEFT_PIN        3     // needs hardware interrupts: UNO pin 2, 3 | MEGA pin 2, 3, 18, 19

// Line tracking (KY-033)
#define LINE_TRACKING_LEFT_PIN    2
#define LINE_TRACKING_MIDDLE_PIN  4
#define LINE_TRACKING_RIGHT_PIN   10

// DC Motor (L298)
// - the two left motors are connected together and also the two right motors
// - left forward:   MOTOR_INX_LEFT_PIN=HIGH  & MOTOR_INY_LEFT_PIN=LOW
// - left backward:  MOTOR_INX_LEFT_PIN=LOW   & MOTOR_INY_LEFT_PIN=HIGH
// - right forward:  MOTOR_INX_RIGHT_PIN=LOW  & MOTOR_INY_RIGHT_PIN=HIGH
// - right backward: MOTOR_INX_RIGHT_PIN=HIGH & MOTOR_INY_RIGHT_PIN=LOW
#define MOTOR_EN_LEFT_PIN         5     // Left  wheel speed
#define MOTOR_EN_RIGHT_PIN        6     // Right wheel speed
#define MOTOR_INX_LEFT_PIN        7     // Left  wheel IN4
#define MOTOR_INY_LEFT_PIN        8     // Left  wheel IN3
#define MOTOR_INX_RIGHT_PIN       9     // Right wheel IN2
#define MOTOR_INY_RIGHT_PIN       11    // Right wheel IN1


// Elegoo
#elif ROBOTCAR_MODEL == 2

// 1: use it, 0: do not use it
#define USE_LCD                   false
#define USE_IR_RECEIVER           true
#define USE_BLUETOOTH             false
#define USE_PS2_CONTROLLER        false
#define USE_LINE_TRACKING         true
#define USE_MOTOR_AUTO            false // needs: USE_SERVO + USE_ULTRASONIC_SENSOR / USE_VL53L0X_SENSOR + USE_PHOTO_ENC
#define USE_SERVO                 false
#define USE_ULTRASONIC_SENSOR     false // attention: cannot enable both USE_ULTRASONIC_SENSOR and USE_VL53L0X_SENSOR
#define USE_VL53L0X_SENSOR        false // attention: cannot enable both USE_ULTRASONIC_SENSOR and USE_VL53L0X_SENSOR
#define USE_PHOTO_ENC             false

// IR Receiver
#define IR_RECEIVE_PIN            12    // module: G=GND, R=5V, Y=Signal ; receiver front: 1=Signal, 2=GND, 3=5V

// Bluetooth
#define BLUETOOTH_SERIAL          Serial // Serial, Serial1, Serial2 or Serial3
#define BLUETOOTH_SERIAL_SPEED    9600

// PS2 controller
#define PS2_DAT_PIN               A0
#define PS2_CMD_PIN               A1
#define PS2_SEL_PIN               A2
#define PS2_CLK_PIN               A3

// Servo
#define SERVO_PIN                 3     // on boards other than the Mega, use of the library disables analogWrite() (PWM)
                                        // functionality on pins 9 and 10, whether or not there is a Servo on those pins. 

// Ultrasonic sensor (HC-SR04)
#define ULTRASONIC_ECHO_PIN       A4
#define ULTRASONIC_TRIG_PIN       A5

// Photo electric encoder (HC-020K)
#define PHOTO_ENC_RIGHT_PIN       2     // needs hardware interrupts: UNO pin 2, 3 | MEGA pin 2, 3, 18, 19
#define PHOTO_ENC_LEFT_PIN        3     // needs hardware interrupts: UNO pin 2, 3 | MEGA pin 2, 3, 18, 19

// Line tracking (KY-033)
#define LINE_TRACKING_LEFT_PIN    2
#define LINE_TRACKING_MIDDLE_PIN  4
#define LINE_TRACKING_RIGHT_PIN   10

// DC Motor (L298)
// - the two left motors are connected together and also the two right motors
// - left forward:   MOTOR_INX_LEFT_PIN=HIGH  & MOTOR_INY_LEFT_PIN=LOW
// - left backward:  MOTOR_INX_LEFT_PIN=LOW   & MOTOR_INY_LEFT_PIN=HIGH
// - right forward:  MOTOR_INX_RIGHT_PIN=LOW  & MOTOR_INY_RIGHT_PIN=HIGH
// - right backward: MOTOR_INX_RIGHT_PIN=HIGH & MOTOR_INY_RIGHT_PIN=LOW
#define MOTOR_EN_LEFT_PIN         5     // Left  wheel speed
#define MOTOR_EN_RIGHT_PIN        6     // Right wheel speed
#define MOTOR_INX_LEFT_PIN        7     // Left  wheel IN4
#define MOTOR_INY_LEFT_PIN        8     // Left  wheel IN3
#define MOTOR_INX_RIGHT_PIN       9     // Right wheel IN2
#define MOTOR_INY_RIGHT_PIN       11    // Right wheel IN1

#endif

#endif
