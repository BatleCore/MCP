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
#define PORT_JOYLEFT PORTK
#define DDR_JOYLEFT DDRK
#define PORT_JOYRIGHT PORTF
#define DDR_JOYRIGHT DDRF


// custom functions:
void print_to_lcd(char * msg1, int16_t val1, int16_t val2);
void send_uart2_string(char* message);
void USART2_init();
void send_packet(uint8_t* data);


#endif