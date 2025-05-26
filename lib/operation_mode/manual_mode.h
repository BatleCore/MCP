#ifndef MANUAL_MODE_H_ //double inclusion guard
#define MANUAL_MODE_H_

//include standard libraries
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <util/delay.h>
#include <stdio.h>

//include provided header files
#include "serial.h" //minimal serial lib
#include "adc.h" //minimal adc lib
#include "milliseconds.h" //milliseconds timekeeping lib

//include custom header files
#include "PIN_ASSIGNMENT.h"
#include "communication.h"
#include "motor_control.h"
#include "servo_control.h"

// global variables
extern uint8_t current_mode;

//functions
void manual_loop();

#endif