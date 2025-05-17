#include "servo_control.h"

static uint8_t velocity = 0;
static uint8_t direction = 0;

void servo_init() {
// set motor output pins

  // Set PWM timer for motors
  TCCR3A = 0;
  TCCR3B = 0;

  TCCR3A = (1<<COM3A1)|(1<<COM3B1);   // Non-inverting PWM on OC1A and OC1B
  TCCR3B = (1 << WGM33) | (1 << CS31);  // Mode 8: PWM Phase & Freq Correct, Prescaler = 8
  ICR3 = 20000;              // Set TOP for 50Hz
  TIMSK3 |= (1<<TOIE3); // Bit 0 – TOIEn: Timer/Countern, Overflow Interrupt Enable
  OCR3A = SERVO_PULSE_OPEN;
  DDR_SERVO|= (1 << PIN_SERVO); // Set PWM pins as output
  gripper_toggle(); // initializes gripper to open state ( try using static file-scope variable? )
}

void servo_set_pos(uint16_t pos) {
  if (pos > SERVO_PULSE_OPEN) {
    SERVO_POS = SERVO_PULSE_OPEN;
  } else if (pos < SERVO_PULSE_CLOSE) {
    SERVO_POS = SERVO_PULSE_OPEN;
  } else {
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
  static int gripper_state = 1; // 0: open, 1: closed

  if (gripper_state) { // if closed
    // open it
    gripper_state = 0;
    SERVO_POS = SERVO_PULSE_OPEN;

  } else { // is open
    gripper_state = 1;
    SERVO_POS = SERVO_PULSE_CLOSE;
  }
}

void servo_set_velocity(uint8_t* servo_data) {
  velocity = servo_data[0];
  velocity = servo_data[1];
}

void servo_read_joystick(uint8_t* servo_data) {
  // servo_data[0]: speed
  // servo_data[1]: direction

  uint16_t speed;
  uint16_t direction;
  speed = adc_read(PIN_JOY_R_Y);

  int centre_BOT = 512 * 0.95;
  int centre_TOP = 1023 - centre_BOT;
  int hyst_range = centre_TOP - centre_BOT;
  int true_range = 1023 - hyst_range;

  /* trim speed value */
  if (speed > centre_TOP)
  {
    servo_data[0] = (speed - centre_TOP) * (15/centre_BOT) + 1;
    servo_data[1] = 2;
  }
  else if (speed > centre_BOT)
  {
    servo_data[0] = 0;
    servo_data[1] = 2;
  } else {
    servo_data[0] = centre_BOT - speed * (15/centre_BOT) + 1;
    servo_data[1] = 0;
  }

}


// TIMSK1 |= (1<<TOIE1); // Bit 0 – TOIEn: Timer/Countern, Overflow Interrupt Enable
ISR(TIMER1_OVF_vect) {
  uint16_t new_val = SERVO_POS + velocity * direction;
  servo_set_pos(new_val);
}