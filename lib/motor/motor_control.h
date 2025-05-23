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
#define HYST 0.05
#define TURNING_CAP 1 // 1 is good for auto, manual steering isnt so smooth though

#define SPEED_INVERT 0
#define TURN_INVERT 0

#define MOTOR_MAX 250
#define MOTOR_AUTO_SPEED 250
#define MOTOR_AUTO_TURN 250

// CLOCK COMPLARE FOR PWMs
#define DUTY_LEFT OCR1A
#define DUTY_RIGHT OCR1B

#define PORT_PWM PORTB
#define DDR_PWM DDRB

#define PORT_CONTROL PORTA
#define DDR_CONTROL DDRA

//functions
void motor_data_conversion(uint8_t* results); // to be removed. broken into: rs_motor_conversion, cs_motor_conversion
void motor_init();
void motor_init_params();
void differential_PWM_v3(uint8_t* motor_data); // to be cleaned / renamed
void motor_test();
void timerPWM_init(); // not used? double check
void rs_motor_conversion();
void cs_motor_conversion(uint8_t* results);
void motor_fromSerial(uint8_t* motor_serial);
void motor_stop();
void motor_straight_forward();
void motor_left_forward();
void motor_right_forward();
void motor_turn_forward(int turn_dir);
void motor_turn_spot(int turn_dir);
void motor_turn_modifier(int turn_dir);

//scope-global variables
int centre_BOT;
int centre_TOP;
int hyst_range;
int true_range;
// uint8_t motor_data_scope[4] = {0}; // this should replace motor_data in all files. auto mode should write to this, not have its own.

#endif