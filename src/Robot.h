#ifndef CONTROLLER_H_ //double inclusion guard
#define CONTROLLER_H_

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

#include "battery.h"
#include "communication.h"
#include "controller_display.h"
#include "light_sensor.h"
#include "motor_control.h"
#include "servo_control.h"
#include "auto_mode.h"
#include "manual_mode.h"

//constants
#define BUILD_DATE __TIME__ " " __DATE__"\n"
#define AUTO_MODE 0
#define MANUAL_MODE 1

//variables - scope/global

//functions
void setup();
int main(void);
void serial_forAuto();

#endif