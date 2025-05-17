#ifndef BATTERY_H_
#define BATTERY_H_

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
#include "PIN_ASSIGNMENT.h"

// === Constants and Configuration ===
#define PORT_BATTERY PORTA  
#define DDR_BATTERY_LED DDRA
#define BATTERY_THRESH 950    // seems to trip at 7.4v supply..?
#define CHECK_TIME 100
#define FLASH_TIME 25

// === Global Variables ===
volatile bat_val;             // stores battery voltage reading
volatile uint8_t battery_check_counter;
volatile uint8_t battery_flash_counter;
volatile bool bat_low;

// === Functions ===
void battery_init();
void testBattery();

#endif /* BATTERY_H_ */
