#ifndef LIGHT_SENSOR_H_
#define LIGHT_SENSOR_H_

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
#include "hd44780.h" //LCD lib

//include custom header files
#include"PIN_ASSIGNMENT.h"

//constants

//functions
void LDR_init();
void readLDR();
uint16_t on_adc_sample(uint16_t adc);
void LDR_test();

#endif