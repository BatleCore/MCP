#include "motor_control.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

void motor_init() {
  // set motor output pins
  DDR_CONTROL |= (1<<PIN_ML_F)|(1<<PIN_ML_R)|(1<<PIN_MR_F)|(1<<PIN_MR_R); // only using 2 of these

  // Set PWM timer for motors
  TCCR1A = (1<<COM1A1)|(1<<COM1B1);   // Non-inverting PWM on OC1A and OC1B
  TCCR1B = (1 << WGM13) | (1 << CS11);  // Mode 8: PWM Phase & Freq Correct, Prescaler = 8
  ICR1 = 2000;              // Set TOP for 500Hz
  OCR1A = 0;
  OCR1B = 0;
  DDR_PWM |= (1 << PIN_PWM_ML) | (1 << PIN_PWM_MR); // Set PWM pins as output

  // lock motors to off state
  OCR1A = 0;
  PORT_CONTROL &= ~(1 << PIN_ML_F); // LOW
  PORT_CONTROL &= ~(1 << PIN_ML_R); // LOW
  OCR1B = 0;
  PORT_CONTROL &= ~(1 << PIN_MR_F); // LOW
  PORT_CONTROL &= ~(1 << PIN_MR_R); // LOW

}

void motor_data_conversion(int speed, int turning, uint8_t* results, int* bug)
{
  static float hyst = 0.1;
  static float turning_cap = 1;

  /* Initialization */
  // top value = 1023
  // centre val = 512

  // static int results[4]; // [left_duty, left_dir, right_duty, right_dir]

  int centre_BOT = 512 * (1 - hyst);
  int centre_TOP = 1023 - centre_BOT;
  int hyst_range = centre_TOP - centre_BOT;
  int true_range = 1023 - hyst_range;

  printf("centre TOP: %d\n", centre_TOP);
  printf("centre BOT: %d\n", centre_BOT);
  printf("true range: %d\n", true_range);
  printf("hyst range: %d\n", hyst_range);
  /* define variables */
  float turn_magnitude = 0; // [ 0 -> 1 ]
  int turn_direction = 0;   // [ 0, 1 ] - 0 for left?

  // int fast_dir = 1; // default forward
  // int slow_dir = 1; // default forward
  // fast_duty - defined later / no default
  // slow_duty - defined later / no default

  // quick nasty fix
  // speed 0 = forward
  // speed 1023 = backward

  speed = 1023 - speed;

  /* trim speed value */
  if (speed > centre_TOP)
  {
    speed = speed - hyst_range;
  }
  else if (speed > centre_BOT)
  {
    speed = centre_BOT;
  }
  printf("trimmed speed value: %d\n", speed);

  /* trim turning value */
  if (turning > centre_TOP)
  {
    turn_magnitude = (turning - centre_TOP) * turning_cap / centre_BOT;
    turn_direction = 1;
  }
  else if (turning < centre_BOT)
  {
    turn_magnitude = (centre_BOT - turning) * turning_cap / centre_BOT;
  }

  /* split left and right */
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

  // int fast_duty = abs((fast_side - centre_BOT) * 254 / centre_BOT);
  // int fast_dir = (fast_side>centre_BOT);

  // int slow_duty = abs((slow_side - centre_BOT) * 254 / centre_BOT);
  // int slow_dir = (slow_side>centre_BOT);


  // RESULTS MAY NEED TO BE RETURNED AS uint8_t
  // as unsigned ints, -1 is useless, therefore:
  // forward = 2
  // not moving = 1
  // reverse = 0
  // results[4]; // [left_duty, left_dir, right_duty, right_dir]

  // turn direction:
  //    0: turn left
  //    1: turn right
  if (turn_direction == 0) // toggle [0, 1] to correct direction
  {
    // Left side slower
    results[0] = abs((slow_side - centre_BOT) * 250 / centre_BOT);
    results[1] = ((int)slow_side > centre_BOT) ? 2 : ((int)slow_side < centre_BOT) ? 0 : 1;
    // right side faster
    results[2] = abs((fast_side - centre_BOT) * 250 / centre_BOT);
    results[3] = ((int)fast_side > centre_BOT) ? 2 : ((int)fast_side < centre_BOT) ? 0 : 1;
  }
  else
  {
    // left side faster
    results[2] = abs((slow_side - centre_BOT) * 250 / centre_BOT);
    results[3] = ((int)slow_side > centre_BOT) ? 2 : ((int)slow_side < centre_BOT) ? 0 : 1;
    // right side slower
    results[0] = abs((fast_side - centre_BOT) * 250 / centre_BOT);
    results[1] = ((int)fast_side > centre_BOT) ? 2 : ((int)fast_side < centre_BOT) ? 0 : 1;
  }
  bug[0] = slow_side;
  bug[1] = fast_side;
  bug[2] = (int)(turn_magnitude*1000);
  bug[3] = speed;
  bug[4] = turn_direction;
  // output = results;
  // return pointer to first arrary location
  // return *results[0];
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

void differential_PWM_init(){
  TCCR1A = (1<<COM1A1)|(1<<COM1B1);   // Non-inverting PWM on OC1A and OC1B
  TCCR1B = (1 << WGM13) | (1 << CS11);  // Mode 8: PWM Phase & Freq Correct, Prescaler = 8
  ICR1 = 2000;              // Set TOP for 500Hz
  OCR1A = 0;
  OCR1B = 0;
  DDR_PWM |= (1 << PIN_PWM_ML) | (1 << PIN_PWM_MR); // Set PWM pins as output
  
  // lock motors to off state
  OCR1A = 0;
  PORT_CONTROL &= ~(1 << PIN_ML); // LOW
  OCR1B = 0;
  PORT_CONTROL &= ~(1 << PIN_MR); // LOW
}