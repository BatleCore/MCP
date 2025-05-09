#ifndef PIN_ASSIGNMENT_H_
#define PIN_ASSIGNMENT_H_

#include <avr/io.h>

// INPUTS - DIGITAL

// INPUTS - ANALOG
#define PIN_BATTERY_SENSE PF0 // D54
#define PIN_LDR_LEFT PF1 // D55
#define PIN_LDR_RIGHT PF2 // D56
#define PIN_DISTANCE_LEFT PF3 // D57
#define PIN_DISTANCE_CENT PF4 // D58
#define PIN_DISTANCE_RIGHT PF5 // D59

// OUTPUTS - DIGITAL
#define PIN_ML_F PA0 // D22 → Left motor forward
#define PIN_ML_R PA1 // D23 → Left motor reverse
#define PIN_MR_F PA2 // D24 → Right motor forward
#define PIN_MR_R PA3 // D25 → Right motor reverse
#define PINBATTERY_LED PA4 // D26

// OUTPUTS - PWM
#define PIN_PWM_ML PB5 // D11 - OC1A
#define PIN_PWM_MR PB6 // D12 - OC1B
#define PIN_PWM_SERVO PE3 // D5 - OC3A






#endif