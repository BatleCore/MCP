#ifndef CONTROLLER_H_ //double inclusion guard
#define CONTROLLER_H_

//include standard libraries
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

//include provided header files
#include "serial.h" //minimal serial lib
#include "adc.h" //minimal adc lib
#include "milliseconds.h" //milliseconds timekeeping lib
#include "hd44780.h" //LCD lib

//include custom header files
#include "PIN_ASSIGNMENT.h"
#include "communication.h"
#include "motor_control.h"
#include "servo_control.h"
#include "controller_display.h"

//constants
#define BUILD_DATE __TIME__ " " __DATE__"\n"

#define AUTO_MODE 0
#define MANUAL_MODE 1
#define LCD_RATE 1000 // 250ms 4hz
#define CONTROL_RATE 20 // 20ms 50hz




// custom functions:
void controller_setup();
int main(void);



//variables - scope/global
uint8_t robot_mode;


#endif