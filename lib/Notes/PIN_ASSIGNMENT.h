#ifndef PIN_ASSIGNMENT_H_
#define PIN_ASSIGNMENT_H_

#include <avr/io.h>

// INPUTS - DIGITAL

// INPUTS - ANALOG
#define PIN_BATTERY_SENSE 0 // D54
#define PIN_LDR_LEFT 1 // D55
#define PIN_LDR_RIGHT 2 // D56
#define PIN_DISTANCE_LEFT 3 // D57
#define PIN_DISTANCE_CENT 4 // D58
#define PIN_DISTANCE_RIGHT 5 // D59

#define PIN_JOY_L_X 14 // A14
#define PIN_JOY_L_Y 15 // A15
#define PIN_JOY_R_X PF1
#define PIN_JOY_R_Y PF0

// OUTPUTS - DIGITAL
#define PIN_ML_F PA0 // D22 → Left motor forward
#define PIN_ML_R PA1 // D23 → Left motor reverse
#define PIN_MR_F PA2 // D24 → Right motor forward
#define PIN_MR_R PA3 // D25 → Right motor reverse
#define PIN_BATTERY_LED PA4   // Indicator LED digital output

#define PWM_LEFT OCR1B
#define PWM_RIGHT OCR1A


// OUTPUTS - PWM
#define PIN_PWM_ML PB5 // D11 - OC1A
#define PIN_PWM_MR PB6 // D12 - OC1B
#define PIN_PWM_SERVO PE3 // D5 - OC3A






#endif