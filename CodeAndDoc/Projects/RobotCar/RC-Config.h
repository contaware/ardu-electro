/*  
  Configuration file
*/ 
#ifndef rc_config_h
#define rc_config_h

#include <Arduino.h>


// Serial Debug
// - if USE_DPRINT is set to true, DPRINT, DPRINTLN, ... do output to Serial Monitor.
// - if USE_DPRINT is set to false, DPRINT, DPRINTLN, ... are optimized away.
#define USE_DPRINT                    true
#define DPRINT_SERIAL_SPEED           115200
#define DPRINT(...)                   do { if (USE_DPRINT) Serial.print(__VA_ARGS__); } while (false)
#define DPRINTLN(...)                 do { if (USE_DPRINT) Serial.println(__VA_ARGS__); } while (false)
#define DWRITE(...)                   do { if (USE_DPRINT) Serial.write(__VA_ARGS__); } while (false)
// Note: do-while(false) guards against if-else constructs without curly braces.


// Choose the setup for your Robot Car
#define ROBOTCAR_MODEL                1


#if ROBOTCAR_MODEL == 1

// 1: use it, 0: do not use it
#define USE_LCD                       false
#define USE_SERVO                     false
#define USE_ULTRASONIC_SENSOR         true
#define USE_PHOTO_ENC                 false

// IR Receiver
#define IR_RECEIVE_PIN                12      // module: G=GND, R=5V, Y=Signal ; receiver front: 1=Signal, 2=GND, 3=5V

// Servo
#define SERVO_PIN                     3       // on boards other than the Mega, use of the library disables analogWrite() (PWM)
                                              // functionality on pins 9 and 10, whether or not there is a Servo on those pins. 

// Ultrasonic sensor (HC-SR04)
#define ULTRASONIC_ECHO_PIN           A4
#define ULTRASONIC_TRIG_PIN           A5
#define ULTRASONIC_WALL_BRAKE_CM      20.0
#define ULTRASONIC_WALL_RELEASE_CM    30.0

// Photo electric encoder (HC-020K)
#define PHOTO_ENC_RIGHT_PIN           2       // needs hardware interrupts: UNO pin 2, 3 | MEGA pin 2, 3, 18, 19
#define PHOTO_ENC_LEFT_PIN            3       // needs hardware interrupts: UNO pin 2, 3 | MEGA pin 2, 3, 18, 19

// DC Motor (L298)
// - for 4 wheels versions the two left motors are connected together and also the two right motors
// - left forward:   MOTOR_INX_LEFT_PIN=HIGH  & MOTOR_INY_LEFT_PIN=LOW
// - left backward:  MOTOR_INX_LEFT_PIN=LOW   & MOTOR_INY_LEFT_PIN=HIGH
// - right forward:  MOTOR_INX_RIGHT_PIN=LOW  & MOTOR_INY_RIGHT_PIN=HIGH
// - right backward: MOTOR_INX_RIGHT_PIN=HIGH & MOTOR_INY_RIGHT_PIN=LOW
#define MOTOR_EN_LEFT_PIN             5       // Left  wheel speed
#define MOTOR_EN_RIGHT_PIN            6       // Right wheel speed
#define MOTOR_INX_LEFT_PIN            7       // Left  wheel IN4
#define MOTOR_INY_LEFT_PIN            8       // Left  wheel IN3
#define MOTOR_INX_RIGHT_PIN           9       // Right wheel IN2
#define MOTOR_INY_RIGHT_PIN           11      // Right wheel IN1


#elif ROBOTCAR_MODEL == 2

// 1: use it, 0: do not use it
#define USE_LCD                       true
#define USE_SERVO                     true
#define USE_ULTRASONIC_SENSOR         true
#define USE_PHOTO_ENC                 true

// IR Receiver
#define IR_RECEIVE_PIN                49      // module: G=GND, R=5V, Y=Signal ; receiver front: 1=Signal, 2=GND, 3=5V

// Servo
#define SERVO_PIN                     10      // on boards other than the Mega, use of the library disables analogWrite() (PWM)
                                              // functionality on pins 9 and 10, whether or not there is a Servo on those pins.

// Ultrasonic sensor (HC-SR04)
#define ULTRASONIC_ECHO_PIN           A2
#define ULTRASONIC_TRIG_PIN           A3
#define ULTRASONIC_WALL_BRAKE_CM      30.0
#define ULTRASONIC_WALL_RELEASE_CM    40.0

// Photo electric encoder (HC-020K)
#define PHOTO_ENC_RIGHT_PIN           2       // needs hardware interrupts: UNO pin 2, 3 | MEGA pin 2, 3, 18, 19
#define PHOTO_ENC_LEFT_PIN            3       // needs hardware interrupts: UNO pin 2, 3 | MEGA pin 2, 3, 18, 19

// DC Motor (L298)
// - for 4 wheels versions the two left motors are connected together and also the two right motors
// - left forward:   MOTOR_INX_LEFT_PIN=HIGH  & MOTOR_INY_LEFT_PIN=LOW
// - left backward:  MOTOR_INX_LEFT_PIN=LOW   & MOTOR_INY_LEFT_PIN=HIGH
// - right forward:  MOTOR_INX_RIGHT_PIN=LOW  & MOTOR_INY_RIGHT_PIN=HIGH
// - right backward: MOTOR_INX_RIGHT_PIN=HIGH & MOTOR_INY_RIGHT_PIN=LOW
#define MOTOR_EN_LEFT_PIN             5       // Left  wheel speed
#define MOTOR_EN_RIGHT_PIN            6       // Right wheel speed
#define MOTOR_INX_LEFT_PIN            7       // Left  wheel IN4
#define MOTOR_INY_LEFT_PIN            8       // Left  wheel IN3
#define MOTOR_INX_RIGHT_PIN           9       // Right wheel IN2
#define MOTOR_INY_RIGHT_PIN           11      // Right wheel IN1

#endif

#endif
