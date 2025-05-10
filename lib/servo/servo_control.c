#include "servo_control.h"

/*
servo OPEN: 1860
servo CLOSE: 1440
*/
void servo_init() {
// set motor output pins

  // Set PWM timer for motors
  TCCR3A = 0;
  TCCR3B = 0;

  TCCR3A = (1<<COM3A1)|(1<<COM3B1);   // Non-inverting PWM on OC1A and OC1B
  TCCR3B = (1 << WGM33) | (1 << CS31);  // Mode 8: PWM Phase & Freq Correct, Prescaler = 8
  ICR3 = 20000;              // Set TOP for 500Hz
  OCR3A = SERVO_PULSE_OPEN;
  DDR_SERVO|= (1 << PIN_SERVO); // Set PWM pins as output
}

void servo_set_pos(uint16_t pos) {
  if(pos >= 0 && pos <= 2420) {
    SERVO_POS = pos;
  }
}

void servo_callibration(void) {
  // used for testing the servo positions
  //potentiometer is connected to ADC pin PA7
  int control_pin = PA7;

  servo_init();
  serial0_init();
  char msg[20];
  adc_init();
  _delay_ms(20);
  int control_val;
  int adc_val;

  // loop
  while (1) {
    adc_val = adc_read(control_pin);
    control_val = (adc_val / 1023.0) * SERVO_PULSE_RANGE + SERVO_PULSE_MIN;
    servo_set_pos(control_val);
    sprintf(msg, "\nADC: %d\n", adc_val);
    serial0_print_string(msg);
    sprintf(msg, "\n%d microseconds\n", control_val);
    serial0_print_string(msg);
    _delay_ms(100);
  }

}

void gripper_toggle() {
  static int gripper_state = 0; // 0: copen, 1: closed

  if (gripper_state) { // if closed
    // open it
    gripper_state = 0;
    SERVO_POS = SERVO_PULSE_OPEN;

  } else { // is open
    gripper_state = 1;
    SERVO_POS = SERVO_PULSE_CLOSE;
  }
}