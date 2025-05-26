#ifndef PIN_ASSIGNMENT_H_
#define PIN_ASSIGNMENT_H_

#include <avr/io.h>

// INPUTS - DIGITAL
#define PIN_JOY_L_BUTTON PD0    // D21 INT0 [Controller]
#define PIN_JOY_R_BUTTON PD1    // D20 INT1 [Controller]

// INPUTS - ANALOG
#define PIN_BATTERY_SENSE 0     // D54 [Robot]
#define PIN_LDR_LEFT 1          // D55 [Robot]
#define PIN_LDR_RIGHT 2         // D56 [Robot]
#define PIN_DISTANCE_LEFT 3     // D57 [Robot]
#define PIN_DISTANCE_CENT 4     // D58 [Robot]
#define PIN_DISTANCE_RIGHT 5    // D59 [Robot]

#define PIN_JOY_L_X 14  // A14 [Controller]
#define PIN_JOY_L_Y 15  // A15 [Controller]
#define PIN_JOY_R_X 1   // A1  [Controller]
#define PIN_JOY_R_Y 0   // A0  [Controller]

// OUTPUTS - DIGITAL
#define PIN_ML_F PA0 // D22 → Left motor forward    [Robot]
#define PIN_ML_R PA1 // D23 → Left motor reverse    [Robot]
#define PIN_MR_F PA2 // D24 → Right motor forward   [Robot]
#define PIN_MR_R PA3 // D25 → Right motor reverse   [Robot]
#define PIN_BATTERY_LED PA4   // Indicator LED digital output [Robot]

// OUTPUTS - PWM
#define PIN_PWM_ML PB5 // D11 - OC1A [Robot]
#define PIN_PWM_MR PB6 // D12 - OC1B [Robot]
#define PIN_PWM_SERVO PE3 // D5 - OC3A [Robot]

// ISR CONSTANTS
#define PWM_LEFT OCR1B // [Robot]
#define PWM_RIGHT OCR1A // [Robot]

#endif