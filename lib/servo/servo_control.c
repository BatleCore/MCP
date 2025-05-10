#include "servo_control.h"

void servo_init() {
// set motor output pins

  // Set PWM timer for motors
  TCCR3A = 0;
  TCCR3B = 0;

  TCCR3A = (1<<COM3A1)|(1<<COM3B1);   // Non-inverting PWM on OC1A and OC1B
  TCCR3B = (1 << WGM33) | (1 << CS31);  // Mode 8: PWM Phase & Freq Correct, Prescaler = 8
  ICR3 = 20000;              // Set TOP for 500Hz
  OCR3A = 0;
  DDR_SERVO|= (1 << PIN_SERVO); // Set PWM pins as output
}

void servo_set_pos(uint16_t pos) {
  if(pos > 0 && pos < 2420) {
    SERVO_POS = pos;
  }
}