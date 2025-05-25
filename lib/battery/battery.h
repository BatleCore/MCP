#ifndef BATTERY_H_
#define BATTERY_H_

// === Standard AVR Includes ===
#include <avr/io.h>
#include <stdbool.h>
#include <stdio.h>

// === Unit Libraries ===
#include "serial.h"        // Serial lib
#include "adc.h"           // ADC interface
#include "milliseconds.h"  // Timekeeping

// === Custom Libraries ===
#include "PIN_ASSIGNMENT.h"

// === Constants and Configuration ===
#define PORT_BATTERY PORTA  
#define DDR_BATTERY_LED DDRA
#define BATTERY_THRESH 853    // seems to trip at 7.0v supply..?
#define CHECK_TIME 100
#define FLASH_TIME 25

// === Global Variables ===
uint8_t bat_val;             // stores battery voltage reading
bool bat_low;

// === Functions ===
void battery_init();
uint8_t getVoltage();
void testBattery();

#endif
