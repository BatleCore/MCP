#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

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

//constants
#define HYST 0.1
#define TURNING_CAP 1
#define SPEED_INVERT 1
#define TURN_INVERT 1

// CLOCK COMPLARE FOR PWMs
#define DUTY_LEFT OCR1A
#define DUTY_RIGHT OCR1B

#define PORT_PWM PORTB
#define DDR_PWM DDRB

#define PORT_CONTROL PORTA
#define DDR_CONTROL DDRA

//functions
void motor_data_conversion(uint8_t* results);
void motor_init();
void differential_PWM_v3(uint8_t* motor_data);
void motor_test();
void timerPWM_init();
void rs_motor_conversion(uint8_t* input_data);
void cs_motor_conversion(uint8_t* results);

//scope-global variables
int centre_BOT;
int centre_TOP;
int hyst_range;
int true_range;

#endif