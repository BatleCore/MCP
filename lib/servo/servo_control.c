#include "servo_control.h"

static uint8_t velocity = 0;
static uint8_t direction = 0;

uint8_t servo_data[2] = {0}; // this should replace motor_data in all files. auto mode should write to this, not have its own.

char msg[40];

void servo_init() {
  // set motor output pins

  // Set PWM timer for motors
  TCCR3A = 0;
  TCCR3B = 0;

  TCCR3A = (1<<COM3A1)|(1<<COM3B1);   // Non-inverting PWM on OC1A and OC1B
  TCCR3B = (1 << WGM33) | (1 << CS31);  // Mode 8: PWM Phase & Freq Correct, Prescaler = 8
  ICR3 = 20000;              // Set TOP for 50Hz
  // TIMSK3 |= (1<<TOIE3); // Bit 0 – TOIEn: Timer/Countern, Overflow Interrupt Enable
  OCR3A = SERVO_PULSE_OPEN;
  DDR_SERVO|= (1 << PIN_SERVO); // Set PWM pins as output
  servo_set_pos(SERVO_PULSE_OPEN); // default to open
  
  // set ADC filter parameters
  centre_BOT = 512 * (1 - HYST);
  centre_TOP = 1023 - centre_BOT;
  hyst_range = centre_TOP - centre_BOT;
  true_range = 1023 - hyst_range;
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

void gripper_toggle() { // will be obsolete
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

// Robot
void servo_fromSerial(uint8_t* data_fromSerial) {
  sprintf(msg, "0: %d, 1: %d, 2: %d\n", data_fromSerial[0], data_fromSerial[1], data_fromSerial[2]);
  serial0_print_string(msg);
  if (data_fromSerial[2] == 2) {
    servo_set_pos(SERVO_PULSE_CLOSE);
  } else if (data_fromSerial[2] == 0) {
    servo_set_pos(SERVO_PULSE_OPEN);
  }
}

// Controller
void servo_read_joystick() {
  // servo_data[0]: speed
  // servo_data[1]: direction

  uint16_t value;
  value = adc_read(PIN_JOY_R_Y);

  // servo_data[0] is for travel magnitude
  // servo_data[1] is for direction (0: open, 2: close, 1: no change)
  // servo_data[0] is not currently used

  /* trim speed value */
  if (value > centre_TOP)
  {
    // servo_data[0] = (value - centre_TOP) * (15.0/centre_BOT) + 1;
    servo_data[1] = 0;
  }
  else if (value > centre_BOT)
  {
    // servo_data[0] = 0;
    servo_data[1] = 1;
  } else {
    // servo_data[0] = (centre_BOT - value) * (15.0/centre_BOT) + 1;
    servo_data[1] = 2;
  }

}

// Communication function so that controller does all the computing then sends an instruction over serial
// Called when controller wants to update the servo
// sends output (servo position) over serial instead of joystick directions
// Controller
void getServoControl(uint8_t* data_toSerial) {
  servo_read_joystick(); // get newest data
  data_toSerial[0] = servo_data[0];
  data_toSerial[1] = servo_data[1];
}


// TIMSK1 |= (1<<TOIE1); // Bit 0 – TOIEn: Timer/Countern, Overflow Interrupt Enable
// ISR(TIMER1_OVF_vect) {
//   // uint16_t new_val = SERVO_POS + velocity * (direction -1);
//   // servo_set_pos(new_val);
// }