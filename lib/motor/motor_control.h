#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// MOTOR PINS
#define PIN_ML_F PA0 // D22 → Left motor forward
#define PIN_ML_R PA1 // D23 → Left motor reverse
#define PIN_MR_F PA2 // D24 → Right motor forward
#define PIN_MR_R PA3 // D25 → Right motor reverse

// CLOCK COMPLARE FOR PWMs
#define DUTY_LEFT OCR1A
#define DUTY_RIGHT OCR1B

#define PORT_PWM PORTB
#define DDR_PWM DDRB
#define PIN_PWM_ML PB5 // D11 → OC1A (PWM output for left motor)
#define PIN_PWM_MR PB6 // D12 → OC1B (PWM output for right motor)

#define PORT_CONTROL PORTA
#define DDR_CONTROL DDRA

void motor_data_conversion(int speed, int turning, uint8_t* results, int* bug);
void motor_init();
void differential_PWM_v3(uint8_t* motor_data);
void motor_test();

#endif