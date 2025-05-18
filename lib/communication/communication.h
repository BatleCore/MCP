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
#define LDR_REQUEST  0xA0           // Request light sensor data from robot
#define JOYSTICK_MOTOR_READ  0xA1   // Send joystick X/Y to robot for motor control
#define JOYSTICK_SERVO_READ  0xA2   // Send joystick X/Y to robot for servo control
#define RANGE_REQUEST 0xA3          // Request rnage data from robot
#define BATTERY_REQUEST 0xA4
#define REQUEST_ERROR  0xEE  // Error / fallback code (not used here)

// === Global Variables ===


// === Functions ===


#endif /* COMMUNICATION_H_ */
