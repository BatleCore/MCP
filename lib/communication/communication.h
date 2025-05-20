#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

// === Standard AVR Includes ===
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <util/delay.h>
#include <stdio.h>

// === Unit Libraries ===
#include "serial.h"        // Minimal serial lib
#include "adc.h"           // ADC interface
#include "milliseconds.h"  // Timekeeping
#include "hd44780.h"       // LCD interface

// === Custom Libraries ===
#include "battery.h"
#include "light_sensor.h"
#include "dist_sensor.h"
#include "controller_display.h"

// === Constants and Configuration ===
// SERIAL PACKET CODES :
#define LIGHT_REQUEST  0xA0           // Request light sensor data from robot
#define JOYSTICK_MOTOR_READ  0xA1   //superseded // Send joystick X/Y to robot for motor control
#define JOYSTICK_SERVO_READ  0xA2   // superseded // Send joystick X/Y to robot for servo control
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
void sendSwitchOperation();
uint8_t compressADC(uint16_t adr_value);


#endif /* COMMUNICATION_H_ */
