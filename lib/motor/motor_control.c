/* INSTRUCTIONS 4 MAT

void motor_data_conversion(int speed, int turning, uint8_t* results, int* bug)
  Converts joystick-style inputs into motor PWM and direction.
  Inputs:
    speed:    0 (full forward) to 1023 (full reverse)
    turning:  0 (full left) to 1023 (full right)
  Outputs:
    results[4] = {left_duty, left_dir, right_duty, right_dir}
      duty:  0–250 (PWM speed)
      dir:   0 = reverse / left , 1 = forward / right

void differential_PWM_v3(uint8_t* motor_data)
  Applies PWM and direction to motors based on data from motor_data_conversion().
  Expects array of 4 bytes: [L_duty, L_dir, R_duty, R_dir] */

#include "motor_control.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

uint8_t motor_data[4] = {0}; // this should replace motor_data in all files. auto mode should write to this, not have its own.
uint8_t auto_speed_val = AUTO_SPEEDVAL_1;
uint8_t auto_softturn_val = AUTO_STURN_VAL_1;
uint8_t auto_hardturn_val = AUTO_HTURN_VAL_1;


char msg[40];

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
  motor_init_params();
}

// important for controller
void motor_init_params() {
  
    // calculate values for motor functions - these remain constant
    centre_BOT = 512 * (1 - HYST);
    centre_TOP = 1023 - centre_BOT;
    hyst_range = centre_TOP - centre_BOT;
    true_range = 1023 - hyst_range;

}

void motor_data_conversion(uint8_t* results) // no longer used. replaced.
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

  float slow_side_speed_p = ((slow_side - centre_BOT)/ centre_BOT);
  int slow_side_turn = ((int)slow_side > centre_BOT) ? 0 : ((int)slow_side < centre_BOT) ? 2 : 1;
  float fast_side_speed_p = ((fast_side - centre_BOT) / centre_BOT);
  int fast_side_turn = ((int)fast_side > centre_BOT) ? 0 : ((int)fast_side < centre_BOT) ? 2 : 1;

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

  // char msg[20];

  // uint16_t left_duty = 1500 + (motor_data[0]) * (500/250.0);
  // uint8_t left_dir = motor_data[1];
  // uint16_t right_duty = 1500 + (motor_data[2]) * (500/250.0);
  // uint8_t right_dir = motor_data[3];

  // uint16_t left_duty = 1500 + (motor_data[0]) * (500/250.0);
  uint16_t left_duty = (motor_data[0]) ? 1750 + (motor_data[0]) : 0;
  uint8_t left_dir = motor_data[1];
  uint16_t right_duty = (motor_data[2]) ? 1750 + (motor_data[2]) : 0;
  uint8_t right_dir = motor_data[3];

  // sprintf(msg, "\nL: %d\t%d", left_dir, left_duty);
  // serial0_print_string(msg);
  // sprintf(msg, "\nR: %d\t%d", right_dir, right_duty);
  // serial0_print_string(msg);
  // sprintf(msg, "L: %u  \t %u \n", left_duty, right_duty);
  // serial0_print_string(msg);
  
  if (left_duty == 0) { // locked LOW
    PWM_LEFT = 2000; // 100% high, continuous break
    PORT_CONTROL &= ~(1 << PIN_ML_R); // LOW
    PORT_CONTROL &= ~(1 << PIN_ML_F); // LOW
  } else {
    PWM_LEFT = left_duty;
    if ( left_dir ) { // FORWARD
      PORT_CONTROL &= ~(1 << PIN_ML_R);
      PORT_CONTROL |= (1 << PIN_ML_F);
    } else { // REVERSE
      PORT_CONTROL |= (1 << PIN_ML_R);
      PORT_CONTROL &= ~(1 << PIN_ML_F);
    }
  }

  if (right_duty == 0) { // locked LOW
    PWM_RIGHT = 2000; // 100% high, continuous break
    PORT_CONTROL &= ~(1 << PIN_MR_R); // LOW
    PORT_CONTROL &= ~(1 << PIN_MR_F); // LOW
  } else {
    PWM_RIGHT = right_duty;
    if ( right_dir ) { // FORWARD
      PORT_CONTROL &= ~(1 << PIN_MR_R);
      PORT_CONTROL |= (1 << PIN_MR_F);
    } else { // REVERSE
      PORT_CONTROL |= (1 << PIN_MR_R);
      PORT_CONTROL &= ~(1 << PIN_MR_F);
    }
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

void rs_motor_conversion() { // this input is essentially motor_data_scope
  /*
  this is a prototype function.
  it is an offshoot from "motor_data_conversion"
  it is to be run on the robot.
  the goal is simplified auto-mode logic.
  */

  // No output data - controls motors directly
  // char msg[50];

  // sprintf(msg, "\nrs_motor_conversion\nsm: %d\nsd: %d\ntm: %d\ntd: %d\n", input_data[0], input_data[1], input_data[2], input_data[3]);
  // serial0_print_string(msg);

  // resulting data
  uint8_t results[4];

  uint16_t av_speed;

  // this centres the speed value to be +- around 250
  // useful for applying turning modifier

  if (motor_data[1]) { // forward
    av_speed = motor_data[0] + 250;
  } else { // reverse
    av_speed = 250 - motor_data[0];
  }

  // sprintf(msg, "av speed: %d\n", av_speed);
  // serial0_print_string(msg);
  // apply offset for seperate motors

  int fast_side = av_speed + motor_data[2] * TURNING_CAP;
  int slow_side = av_speed - motor_data[2] * TURNING_CAP;

  // sprintf(msg, "fastside: %d\nslowside: %d\n", fast_side, slow_side);
  // serial0_print_string(msg);


  // clamp the speed values

  if (fast_side > 500)
  {
    fast_side = 500;
    slow_side = 500 - 1 * motor_data[2] * TURNING_CAP;
  }
  else if (slow_side < 0)
  {
    slow_side = 0;
    fast_side = 1 * motor_data[2] * TURNING_CAP;
  }

  // sprintf(msg, "clamped\nfastside: %d\nslowside: %d\n", fast_side, slow_side);
  // serial0_print_string(msg);

  float ss_speed_p = (slow_side - 250.0) / 250;
  ss_speed_p = (ss_speed_p > 0) ? ss_speed_p : ss_speed_p * -1;
  int ss_turn_d = (slow_side > 250) ? 1 : 0;
  float fs_speed_p = (fast_side - 250.0) / 250;
  fs_speed_p = (fs_speed_p > 0) ? fs_speed_p : fs_speed_p * -1;
  int fs_turn_d = (fast_side > 250) ? 1 : 0;

  // sprintf(msg, "ssp: %d\nfsp: %d\n", ss_speed_p, fs_speed_p);
  // serial0_print_string(msg);

  // XOR
  // if ( motor_data[3] == motor_data[1]) { 
  if ( !motor_data[3] ) { 
    // Left side slower
    results[0] = ss_speed_p * 250;
    results[1] = ss_turn_d;
    // right side faster
    results[2] = fs_speed_p * 250;
    results[3] = fs_turn_d;
  } else {
    // Left side faster
    results[0] = fs_speed_p * 250;
    results[1] = fs_turn_d;
    // right side slower
    results[2] = ss_speed_p * 250;
    results[3] = ss_turn_d;
  }
  // sprintf(msg, "\nresults\nL spd: %d\nL dir: %d\nR spd: %d\nR dir: %d\n", results[0], results[1], results[2], results[3]);
  // serial0_print_string(msg);

  differential_PWM_v3(results);
}

void cs_motor_conversion(uint8_t* results){
  // no input data - reads ADC pins directly
  // output matches input of "rs_motor_conversion"

  uint8_t travel_mag = 0; // absolute velocity  ( 0 -> 250 )
  uint8_t travel_dir = 0; // velocity direction ( forward, reverse )
  uint8_t turn_mag = 0; // absolute turning ( 0 -> 250 )
  uint8_t turn_dir = 0; // turning direction

  uint16_t speed = adc_read(PIN_JOY_L_Y);
  uint16_t turning = adc_read(PIN_JOY_L_X);

  // sprintf(msg, "speed: %d, turn: %d\n", speed, turning);
  // serial0_print_string(msg);

  // sprintf(msg, "cTOP: %d cBOT: %d\n", centre_TOP, centre_BOT);
  // serial0_print_string(msg);

  /* TRIMMING FOR STICK-DRIFT */
  // revisit exponential steering
  // apply BEFORE steering modifiers

  if (speed > centre_TOP)
  { // forward
    travel_mag = ((speed - centre_TOP*1.0)/centre_BOT) * ((speed - centre_TOP*1.0)/centre_BOT) * (250.0);
    travel_dir = (SPEED_INVERT);
  }
  else if (speed < centre_BOT)
  {
    travel_mag = ((centre_BOT - speed*1.0)/centre_BOT) * ((centre_BOT - speed*1.0)/centre_BOT) * (250.0);
    travel_dir = !(SPEED_INVERT);
  } else {
    travel_mag = 0;
    travel_dir = !(SPEED_INVERT);
  }

  if (turning > centre_TOP)
  { // RIGHT
    turn_mag = ((turning - centre_TOP)*1.0/centre_BOT) * ((turning - centre_TOP*1.0)/centre_BOT) * (250 - travel_mag*0.6);
    turn_dir = (TURN_INVERT);
  }
  else if (turning < centre_BOT)
  { // LEFT
    turn_mag = ((centre_BOT - turning)*1.0/centre_BOT) * ((centre_BOT - turning)*1.0/centre_BOT) * (250 - travel_mag*0.6);
    turn_dir = !(TURN_INVERT);
  } else { 
    turn_mag = 0;
    turn_dir = !(TURN_INVERT);
  }
  // sprintf(msg, "travmag: %d, turnmag: %d\n\n", travel_mag, turn_mag);
  // serial0_print_string(msg);

  results[0] = travel_mag;
  results[1] = travel_dir;
  results[2] = turn_mag;
  results[3] = turn_dir;
}

void motor_fromSerial(uint8_t* motor_serial) {
  for (int i = 0; i < 4; i++) {
    motor_data[i] = motor_serial[i+1];
  }
  rs_motor_conversion();
}

// motor_data macros

void motor_stop(){
  // no motion
  motor_data[0] = 0;
  motor_data[1] = 1; // forward, always forward
  motor_data[2] = 0; // straight
  motor_data[3] = 0; // left  (not significant)
}

void motor_straight_forward(){
  // full speed straight
  motor_data[0] = auto_speed_val;
  motor_data[1] = 1; // forward, always forward
  motor_data[2] = 0; // straight
  motor_data[3] = 0; // left  (not significant)
}

void motor_softturn_forward(int turn_dir){
  // full speed soft turn
  motor_data[0] = auto_speed_val;
  motor_data[1] = 1; // forward, always forward
  motor_data[2] = auto_softturn_val;
  motor_data[3] = turn_dir; // passed as arg, 0 or 1
}

void motor_turn_spot(int turn_dir){
  // no speed hard turn
  motor_data[0] = 0;
  motor_data[1] = 1; // forward, always forward
  motor_data[2] = auto_hardturn_val;
  motor_data[3] = turn_dir; // passed as arg, 0 or 1
}

void motor_hardturn_forward(int turn_dir){
  // full speed hard turn
  motor_data[0] = auto_speed_val;
  motor_data[1] = 1; // forward, always forward
  motor_data[2] = auto_hardturn_val;
  motor_data[3] = turn_dir; // passed as arg, 0 or 1
}

void motor_automode_config(int mode) {
  // mode = 0: maze nav, 1: beacon nav
  if (mode == 0) { // maze
    auto_speed_val = AUTO_SPEEDVAL_1;
    auto_softturn_val = AUTO_STURN_VAL_1;
    auto_hardturn_val = AUTO_HTURN_VAL_1;
  } else if ( mode == 1 ) { // beacon
    auto_speed_val = AUTO_SPEEDVAL_2;
    auto_softturn_val = AUTO_STURN_VAL_2;
    auto_hardturn_val = AUTO_HTURN_VAL_2;
  }
}


void motor_direct_set(int sp_mag, int sp_dir, int turn_mag, int turn_dir) {
  motor_data[0] = sp_mag;
  motor_data[1] = sp_dir;
  motor_data[2] = turn_mag;
  motor_data[3] = turn_dir;
}

void motor_turn_modifier(int turn_dir) { 
  int temp_value;
  if (motor_data[3]) {
    temp_value = 250 + motor_data[2];
  }
  else {
    temp_value = 250 - motor_data[2];
  }
  temp_value += 50 * (-1 + turn_dir * 2);

  if ( temp_value > 250) { 
    motor_data[2] = temp_value - 250;
    motor_data[3] = 1;
  } else {
    motor_data[2] = 250 - temp_value;
    motor_data[3] = 0;
  }
  if (motor_data[2] > 250) {
    motor_data[2] = 250;
  } else if (motor_data[2] < 0) {
    motor_data[2] = 0;
  }
}