/*  
  Configuration file
*/ 
#ifndef rc_config_h
#define rc_config_h

#include <Arduino.h>


// Serial Debug
// Note: when DEBUG is set to false, the compiler will optimize the calls using 
//       DEBUG_SERIAL out of the code because it knows they will never run. 
#define DEBUG true // true: turn on debugging, false: turn off debugging 
#define DEBUG_SERIAL if(DEBUG)Serial


// Mode
extern int g_mode;
extern void setMode(int mode);


// Robot Car Model
#define ROBOTCAR_MODEL            1   // 1: Lafvin, 2: Elegoo


// Lafvin
#if ROBOTCAR_MODEL == 1               

// 1: use it, 0: do not use it
#define USE_LCD                   1
#define USE_IR_RECEIVER           1
#define USE_BLUETOOTH             1
#define USE_PS2_CONTROLLER        1
#define USE_LINE_TRACKING         0
#define USE_MOTOR_AUTO            1   // needs: USE_SERVO + USE_ULTRASONIC_SENSOR / USE_VL53L0X_SENSOR + USE_PHOTO_ENC
#define USE_SERVO                 1
#define USE_ULTRASONIC_SENSOR     0
#define USE_VL53L0X_SENSOR        1
#define USE_PHOTO_ENC             1

// Serial
#define SERIAL_SPEED              9600

// IR Receiver
#define IR_RECEIVE_PIN            12  // module: G=GND, R=5V, Y=Signal ; receiver front: 1=Signal, 2=GND, 3=5V

// PS2 controller
#define PS2_DAT_PIN               A0  // (blue)
#define PS2_CMD_PIN               A1  // (yellow)
#define PS2_SEL_PIN               4   // (green)
#define PS2_CLK_PIN               13  // (grey)

// Servo
#define SERVO_PIN                 10  // (orange)

// Ultrasonic sensor (HC-SR04)
#define ULTRASONIC_ECHO_PIN       A2  // (green)
#define ULTRASONIC_TRIG_PIN       A3  // (yellow)

// Photo electric encoder (HC-020K)
#define PHOTO_ENC_RIGHT_PIN       2   // Attention on UNO only pin 2, 3 work as hardware interrupts (orange)
#define PHOTO_ENC_LEFT_PIN        3   // Attention on UNO only pin 2, 3 work as hardware interrupts (green)

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
#define MOTOR_EN_LEFT_PIN         5   // Left  wheel speed (grey)
#define MOTOR_EN_RIGHT_PIN        6   // Right wheel speed (white)
#define MOTOR_INX_LEFT_PIN        7   // Left  wheel IN4 (blue)
#define MOTOR_INY_LEFT_PIN        8   // Left  wheel IN3 (yellow)            
#define MOTOR_INX_RIGHT_PIN       9   // Right wheel IN2 (brown)
#define MOTOR_INY_RIGHT_PIN       11  // Right wheel IN1 (violet)


// Elegoo
#elif ROBOTCAR_MODEL == 2

// 1: use it, 0: do not use it
#define USE_LCD                   0
#define USE_IR_RECEIVER           1
#define USE_BLUETOOTH             0
#define USE_PS2_CONTROLLER        0
#define USE_LINE_TRACKING         1
#define USE_MOTOR_AUTO            0   // needs: USE_SERVO + USE_ULTRASONIC_SENSOR / USE_VL53L0X_SENSOR + USE_PHOTO_ENC
#define USE_SERVO                 0
#define USE_ULTRASONIC_SENSOR     0
#define USE_VL53L0X_SENSOR        0
#define USE_PHOTO_ENC             0

// Serial
#define SERIAL_SPEED              9600

// IR Receiver
#define IR_RECEIVE_PIN            12

// PS2 controller
#define PS2_DAT_PIN               A0  // (blue)
#define PS2_CMD_PIN               A1  // (yellow)
#define PS2_SEL_PIN               A2  // (green)
#define PS2_CLK_PIN               A3  // (grey)

// Servo
#define SERVO_PIN                 3

// Ultrasonic sensor (HC-SR04)
#define ULTRASONIC_ECHO_PIN       A4  // Attention on UNO that's also I2C SDA
#define ULTRASONIC_TRIG_PIN       A5  // Attention on UNO that's also I2C SCL

// Photo electric encoder (HC-020K)
#define PHOTO_ENC_RIGHT_PIN       2   // Attention on UNO only pin 2, 3 work as hardware interrupts
#define PHOTO_ENC_LEFT_PIN        3   // Attention on UNO only pin 2, 3 work as hardware interrupts

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
#define MOTOR_EN_LEFT_PIN         5   // Left  wheel speed
#define MOTOR_EN_RIGHT_PIN        6   // Right wheel speed
#define MOTOR_INX_LEFT_PIN        7   // Left  wheel IN4
#define MOTOR_INY_LEFT_PIN        8   // Left  wheel IN3            
#define MOTOR_INX_RIGHT_PIN       9   // Right wheel IN2
#define MOTOR_INY_RIGHT_PIN       11  // Right wheel IN1

#endif

#endif
