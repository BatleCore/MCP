#ifndef LIGHT_SENSOR_H_
#define LIGHT_SENSOR_H_

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
#include "motor_control.h"
#include"PIN_ASSIGNMENT.h"

// === Constants and Configuration ===
#define SAMPLE_RATE 100         // 100 Hz sample rate
#define SAMPLE_PERIOD 10        // ms (1000 / SAMPLE_RATE)
#define SIGNAL_THRESHOLD 20     // Signal threshold for edge detection

#define FREQ_TARGET 2000        // 20.00 Hz (in hundredths of Hz)
#define FREQ_TOL    500         // ±5.00 Hz tolerance (hundredths of Hz)

#define PROXIMITY_THRESHOLD 900 // ADC value to trigger "close enough" detection

// === Global Variables ===

// === Functions ===
void LDR_init(void);
uint16_t getFrequency(uint16_t adc, uint8_t channel);
void seekBeacon(uint16_t leftLDR, uint16_t rightLDR);

#endif /* LIGHT_SENSOR_H_ */
