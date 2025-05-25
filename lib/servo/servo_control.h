#ifndef SERVO_CONTROL_H
#define SERVO_CONTROL_H

//include standard libraries
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "serial.h"
#include "adc.h"

//include provided header files


//include custom header files
#include "PIN_ASSIGNMENT.h"

// CLOCK COMPLARE FOR PWMs
#define SERVO_POS OCR3A

#define PORT_SERVO PORTE
#define DDR_SERVO DDRE
#define PIN_SERVO PE3 // D5

//constants
#define SERVO_PULSE_MAX 2360 // us
#define SERVO_PULSE_MIN 620 // us
#define SERVO_PULSE_RANGE 1740
#define SERVO_PULSE_OPEN 1860
#define SERVO_PULSE_CLOSE 1440
#define HYST 0.05

//scope-global variables
int centre_BOT;
int centre_TOP;
int hyst_range;
int true_range;
// uint8_t servo_data[2] = {0}; // this should replace motor_data in all files. auto mode should write to this, not have its own.

//functions
void servo_init();
void servo_set_pos(uint16_t pos);
void servo_callibration(void);
void gripper_toggle();
void servo_fromSerial(uint8_t* data_fromSerial);
void servo_read_joystick();
void getServoControl(uint8_t* data_toSerial);

#endif