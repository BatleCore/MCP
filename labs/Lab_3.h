//Example ATmega2560 Project
//File: ATmega2560Project.h
//Author: Robert Howie

#ifndef CONTROLLER_H_ //double inclusion guard
#define CONTROLLER_H_

//include standard libraries
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <util/delay.h>
#include <stdio.h>

//include header files
#include "serial.h" //minimal serial lib
#include "adc.h" //minimal adc lib
#include "milliseconds.h" //milliseconds timekeeping lib
#include "hd44780.h" //LCD lib


//constants
#define BUILD_DATE __TIME__ " " __DATE__"\n"

#endif /* ATMEGA2560_H_ */

int task_1(void);
int task_2(void);
int task_3(void);
int task_4(void);
int task_5(void);

float ADC_voltage_calc(int ADC_value);
int task_loop(void);
