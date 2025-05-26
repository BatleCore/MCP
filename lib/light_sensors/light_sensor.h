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
#include "PIN_ASSIGNMENT.h"
#include "motor_control.h"
#include "dist_sensor.h"
// #include "auto_mode.h"

// === Constants and Configuration ===
#define BEACON2_FREQ 2000       // 20.00 Hz (in hundredths of Hz)
#define FREQ_TOLERANCE    100   // ±5% tolerance (hundredths of Hz)
#define PROXIMITY_THRESHOLD 900 // ADC value to trigger "close enough" detection
#define SIGNAL_THRESHOLD 15     // Minimum signal to filter ambient false positives
#define SIGNAL_LOW_THRESHOLD 5

// === Global Variables ===
extern volatile uint32_t isr_counter;   // For debugging
volatile uint16_t leftLDR;
volatile uint16_t rightLDR;
volatile int16_t signalLeft;
volatile int16_t signalRight;
volatile uint16_t freqLeft;
volatile uint16_t freqRight;
volatile uint16_t baselineLeft;
volatile uint16_t baselineRight;
uint16_t turn;
uint16_t speed;

// === Functions ===
void LDR_init(void);
uint16_t getLDRval(int pin);
int16_t getSignal(uint16_t LDRval, uint8_t channel);
uint16_t getFrequency(int16_t signal, uint8_t channel);
void seekBeacon();
void getLightValues(uint16_t* light_values);
void getSignals(uint16_t* signal_values);
void getFrequencies(uint16_t* freq_values);
void LDR_test(void);

#endif /* LIGHT_SENSOR_H_ */
