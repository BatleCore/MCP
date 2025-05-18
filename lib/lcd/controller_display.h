// controller display header
#ifndef DISPLAY_H_
#define DISPLAY_H_

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
#include "communication.h"
#include "battery.h"
#include "light_sensor.h"
#include "dist_sensor.h"

// === Constants and Configuration ===

// === Global Variables ===
typedef enum {
    SCREEN_HOME = 0,
    SCREEN_LIGHT,
    SCREEN_RANGE,
    SCREEN_COUNT
} ScreenState;

extern volatile ScreenState currentScreen;

// === Functions ===
void display_init(void);
void nextScreen(void);
void updateLCD();
void test_LCD(ScreenState screen);

#endif /* DISPLAY_H_ */
