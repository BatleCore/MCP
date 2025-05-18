#ifndef LIGHT_SENSOR_H_
#define LIGHT_SENSOR_H_

// === Standard AVR Libraries ===
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

// === Custom Libraries ===
#include "motor_control.h"
#include "PIN_ASSIGNMENT.h"

// === Constants and Configuration ===
#define BEACON2_FREQ 2000       // 20.00 Hz (in hundredths of Hz)
#define FREQ_TOLERANCE    100   // ±5% tolerance (hundredths of Hz)
#define PROXIMITY_THRESHOLD 900 // ADC value to trigger "close enough" detection
#define SIGNAL_THRESHOLD 50     // Minimum signal to filter ambient false positives

// === Global Variables ===
extern volatile uint32_t isr_counter;   // For debugging
extern volatile uint16_t leftLDR;       //
extern volatile uint16_t rightLDR;
volatile int16_t signalLeft;
volatile int16_t signalRight;
extern volatile uint16_t freqLeft;
extern volatile uint16_t freqRight;
uint16_t turn;
uint16_t speed;

// === Functions ===
void LDR_init(void);
uint16_t getLDRval(int pin);
int16_t getSignal(uint16_t LDRval, uint8_t channel);
uint16_t getFrequency(int16_t signal, uint8_t channel);
void seekBeacon(uint16_t leftLDR, uint16_t rightLDR);
void LDR_test(void);

#endif /* LIGHT_SENSOR_H_ */
