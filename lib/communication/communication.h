#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

// === Standard AVR Includes ===
#include <avr/io.h>
#include <stdbool.h>

// === Unit Libraries ===
#include "serial.h"        // Minimal serial lib

// === Custom Libraries ===
#include "battery.h"
#include "light_sensor.h"
#include "dist_sensor.h"
#include "servo_control.h"
#include "motor_control.h"
// #include "auto_mode.h"

// === Constants and Configuration ===
// SERIAL PACKET CODES :
#define LIGHT_REQUEST  0xA0           // Request light sensor data from robot
#define JOYSTICK_MOTOR_READ  0xA1   //superseded by MOTOR_CONTROL // Send joystick X/Y to robot for motor control
#define JOYSTICK_SERVO_READ  0xA2   // superseded by SERVO_CONTROL // Send joystick X/Y to robot for servo control
#define RANGE_REQUEST 0xA3          // Request rnage data from robot
#define BATTERY_REQUEST 0xA4
#define MOTOR_CONTROL 0xB0
#define SERVO_CONTROL 0xB1
#define MODE_SWITCH 0xB2
#define REQUEST_ERROR  0xEE  // Error / fallback code (not used here)

// === Global Variables ===

// === Functions ===
void comms_init();
void requestBATTERYdata();
void sendBATTERYdata();
void requestLIGHTdata();
void sendLIGHTdata();
void requestRANGEdata();
void sendRANGEdata();
void sendMotorControl();
void sendServoControl();
//void sendSwitchOperation();
uint8_t compressADC(uint16_t adr_value);


#endif /* COMMUNICATION_H_ */
