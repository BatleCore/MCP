#ifndef SERVO_CONTROL_H
#define SERVO_CONTROL_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "serial.h"
#include "adc.h"

// CLOCK COMPLARE FOR PWMs
#define SERVO_POS OCR3A

#define PORT_SERVO PORTE
#define DDR_SERVO DDRE
#define PIN_SERVO PE3 // D5

#define SERVO_PULSE_MAX 2360 // us
#define SERVO_PULSE_MIN 620 // us
#define SERVO_PULSE_RANGE 1740
#define SERVO_PULSE_OPEN 1860
#define SERVO_PULSE_CLOSE 1440

void servo_init();
void servo_set_pos(uint16_t pos);
void servo_callibration(void);
void gripper_toggle();

#endif