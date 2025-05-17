/* INSTRUCTIONS 4 MAT

void motor_data_conversion(int speed, int turning, uint8_t* results, int* bug)
  Converts joystick-style inputs into motor PWM and direction.
  Inputs:
    speed:    0 (full forward) to 1023 (full reverse)
    turning:  0 (full left) to 1023 (full right)
  Outputs:
    results[4] = {left_duty, left_dir, right_duty, right_dir}
      duty:  0–250 (PWM speed)
      dir:   0 = reverse, 1 = stop, 2 = forward
    bug[5] (optional): debug info [slow, fast, turn_mag, speed, dir]

void differential_PWM_v3(uint8_t* motor_data)
  Applies PWM and direction to motors based on data from motor_data_conversion().
  Expects array of 4 bytes: [L_duty, L_dir, R_duty, R_dir] */

#include "motor_control.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

void motor_init() {
  // set motor output pins
  DDR_CONTROL |= (1<<PIN_ML_F)|(1<<PIN_ML_R)|(1<<PIN_MR_F)|(1<<PIN_MR_R);

  // Set PWM timer for motors
  TCCR1A = (1<<COM1A1)|(1<<COM1B1);   // Non-inverting PWM on OC1A and OC1B
  TCCR1B = (1 << WGM13) | (1 << CS11);  // Mode 8: PWM Phase & Freq Correct, Prescaler = 8
  ICR1 = 2000;              // Set TOP for 500Hz
  OCR1A = 0;
  OCR1B = 0;
  DDR_PWM |= (1 << PIN_PWM_ML) | (1 << PIN_PWM_MR); // Set PWM pins as output

  // lock motors to off state
  PORT_CONTROL &= ~(1 << PIN_ML_F); // LOW
  PORT_CONTROL &= ~(1 << PIN_ML_R); // LOW
  PORT_CONTROL &= ~(1 << PIN_MR_F); // LOW
  PORT_CONTROL &= ~(1 << PIN_MR_R); // LOW

  // calculate values for motor functions - these remain constant
  centre_BOT = 512 * (1 - HYST);
  centre_TOP = 1023 - centre_BOT;
  hyst_range = centre_TOP - centre_BOT;
  true_range = 1023 - hyst_range;

}


void motor_data_conversion(uint8_t* results)
{
  // INITIALIZING VARIABLES WITH DEFAULT STATES
  float turn_magnitude = 0; // [ 0 -> 1 ]
  int turn_direction = 0;   // [ 0, 1 ] - 0 for left
  int travel_direction = 0; // overall forward or reverse, reverse default

  /* READING ADC VALUES FOR CONTROL INSTRUCTIONS
    SPEED:
      0 IS FORWARD
      1023 IS REVERSE
    TURNING:
      0 IS LEFT
      1023 IS RIGHT
  */
  uint16_t speed = adc_read(PIN_JOY_L_Y);
  uint16_t turning = adc_read(PIN_JOY_L_X);

  speed = 1023 - speed; // dirty fix - joy dirtection invert

  /* TRIMMING FOR STICK-DRIFT */
  if (speed > centre_TOP)
  { // forward
    speed = speed - hyst_range;
    travel_direction = 1;
  }
  else if (speed > centre_BOT)
  {
    speed = centre_BOT;
    // travel_direction = 0 by default
  }

  if (turning > centre_TOP)
  { // RIGHT
    turn_magnitude = (turning - centre_TOP) * TURNING_CAP / centre_BOT;
    turn_direction = 1;
  }
  else if (turning < centre_BOT)
  { // LEFT
    turn_magnitude = (centre_BOT - turning) * TURNING_CAP / centre_BOT;
    // turn_direction = 0 by default
  }

  // skew for two motors
  // lower is faster forward
  float fast_side = speed + turn_magnitude * centre_BOT;
  float slow_side = speed - turn_magnitude * centre_BOT;


  /* clamp value limits */
  if (fast_side > true_range)
  {
    fast_side = true_range;
    slow_side = true_range - 2 * turn_magnitude * centre_BOT;
  }
  else if (slow_side < 0)
  {
    slow_side = 0;
    fast_side = 2 * turn_magnitude * centre_BOT;
  }

  // RESULTS MAY NEED TO BE RETURNED AS uint8_t
  // as unsigned ints, -1 is useless, therefore:
  // forward = 2
  // not moving = 1
  // reverse = 0
  // results[4]; // [left_duty, left_dir, right_duty, right_dir]

  // turn direction:
  //    0: turn left
  //    1: turn right

  int slow_side_speed_p = ((slow_side - centre_BOT)/ centre_BOT);
  int slow_side_turn = ((int)slow_side > centre_BOT) ? 1 : ((int)slow_side < centre_BOT) ? 0 : 2;
  int fast_side_speed_p = ((fast_side - centre_BOT) / centre_BOT);
  int fast_side_turn = ((int)fast_side > centre_BOT) ? 1 : ((int)fast_side < centre_BOT) ? 0 : 2;

  /* THIS IS AN XOR GATE
  __________________
  | D | T || L | R |
  |---|---||---|---|
  | 0 | 0 || S | F | FORWARDS AND LEFT
  | 0 | 1 || F | S | FORWARDS AND RIGHT
  | 1 | 0 || F | S | REVERSE AND LEFT
  | 1 | 1 || S | F | REVERSE AND RIGHT
  
  */
  if (turn_direction == travel_direction)
  {
    // Left side slower
    results[0] = slow_side_speed_p * slow_side_speed_p * 250;
    results[1] = slow_side_turn;
    // right side faster
    results[2] = fast_side_speed_p * fast_side_speed_p * 250;
    results[3] = fast_side_turn;
  }
  else
  {
    // right side faster
    results[2] = ((slow_side - centre_BOT)/ centre_BOT) * ((slow_side - centre_BOT)/ centre_BOT) * 250;
    results[3] = ((int)slow_side > centre_BOT) ? 2 : ((int)slow_side < centre_BOT) ? 0 : 1;
    // Left side slower
    results[0] = ((fast_side - centre_BOT) / centre_BOT) * ((fast_side - centre_BOT) / centre_BOT) * 250;
    results[1] = ((int)fast_side > centre_BOT) ? 2 : ((int)fast_side < centre_BOT) ? 0 : 1;
  }
}

void differential_PWM_v3(uint8_t* motor_data){
  /*
  [0] = left_duty
  [1] = left_dir
  [2] = right_duty
  [3] = right_dir

  dir:
    2 = forward
    1 = stationary
    0 = reverse
  */

  char msg[20];

  // uint16_t left_duty = 1500 + (motor_data[0]) * (500/250.0);
  // uint8_t left_dir = motor_data[1];
  // uint16_t right_duty = 1500 + (motor_data[2]) * (500/250.0);
  // uint8_t right_dir = motor_data[3];

  // uint16_t left_duty = 1500 + (motor_data[0]) * (500/250.0);
  uint16_t left_duty = 1750 + (motor_data[0]);
  uint8_t left_dir = motor_data[1];
  uint16_t right_duty = 1750 + (motor_data[2]);
  uint8_t right_dir = motor_data[3];

  sprintf(msg, "\nL: %d\t%d\n", left_dir, left_duty);
  serial0_print_string(msg);
  sprintf(msg, "R: %d\t%d\n", right_dir, right_duty);
  serial0_print_string(msg);
  // sprintf(msg, "L: %u  \t %u \n", left_duty, right_duty);
  // serial0_print_string(msg);
  OCR1A = left_duty;
  if( left_dir == 2) { // Forward
    PORT_CONTROL &= ~(1 << PIN_ML_R);
    PORT_CONTROL |= (1 << PIN_ML_F);
  } else if ( left_dir == 0 ) { // Reverse
    PORT_CONTROL |= (1 << PIN_ML_R);
    PORT_CONTROL &= ~(1 << PIN_ML_F);
  } else { // locked LOW
    OCR1A = 0;
    PORT_CONTROL &= ~(1 << PIN_ML_R);
    PORT_CONTROL &= ~(1 << PIN_ML_F);
  }

  OCR1B = right_duty;
  if( right_dir == 2 ) { // Forward
    PORT_CONTROL &= ~(1 << PIN_MR_R);
    PORT_CONTROL |= (1 << PIN_MR_F);
  } else if ( right_dir == 0 ) { // Reverse
    PORT_CONTROL |= (1 << PIN_MR_R);
    PORT_CONTROL &= ~(1 << PIN_MR_F);
  } else { // locked LOW
    OCR1B = 0;
    PORT_CONTROL &= ~(1 << PIN_MR_R);
    PORT_CONTROL &= ~(1 << PIN_MR_F);
  }
}

void motor_test() {
  serial0_init();
  static int init = 1;
  if(init) {
    motor_init();
    init = 0;
  }
  uint8_t motor_data[4];
  
  motor_data[0] = 0;
  motor_data[1] = 2;
  motor_data[2] = 0;
  motor_data[3] = 2;
  
  for(uint8_t i = 0; i< 250; i++){
    motor_data[0] = i;
    motor_data[2] = i;
    differential_PWM_v3(motor_data);
    _delay_ms(50);
  }
  motor_data[0] = 0;
  motor_data[1] = 1;
  motor_data[2] = 0;
  motor_data[3] = 1;
  differential_PWM_v3(motor_data);
  while(1){

  }

}

/********************
Timer1 PWM Setup
Phase and Frequency Correct PWM Mode (mode 8)
Prescaler = 8, ICR1 = 2000 → 500Hz PWM frequency
Used for motor speed control
********************/
void timerPWM_init() {
  /*
  PWM MAX = 2000
  PWM freq = 500Hz
  PWM period = 2ms
  */
  TCCR1A = (1<<COM1A1)|(1<<COM1B1);   // Non-inverting PWM on OC1A and OC1B
  TCCR1B = (1 << WGM13) | (1 << CS11);  // Mode 8: PWM Phase & Freq Correct, Prescaler = 8
  ICR1 = 2000;              // Set TOP for 500Hz
  OCR1A = 0;
  OCR1B = 0;
  DDR_PWM |= (1 << PIN_PWM_ML) | (1 << PIN_PWM_MR); // Set PWM pins as output
}

