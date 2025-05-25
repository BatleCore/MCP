#include "auto_mode.h"

#define FRONT_HARD_LIM_1 120
#define FRONT_HARD_LIM_2 175
#define FRONT_SOFT_LIM_1 225
#define FRONT_SOFT_LIM_2 225
#define SIDE_HARD_LIM 120

uint16_t distance_values[3] = {0}; // initialize all to 0

int front_hard_lim = FRONT_HARD_LIM_1; // hard limit - dont go forward
int front_soft_lim = FRONT_SOFT_LIM_1; // soft limit - begin wide turn
int side_hard_lim = SIDE_HARD_LIM;
int previous_turn_direction = 1;
int turn_bias = 2;
char msg[50];

void auto_init() {
  // set servo to 'open'
  servo_set_pos(SERVO_PULSE_OPEN);
}

void mode_5() {
  // currently a 3/5
  // 4: no jitter
  // 5: no hesitation 

  get_distances(distance_values);
  
  int left = distance_values[0];
  int centre = distance_values[1];
  int right = distance_values[2];

  sprintf(msg, "\nL: %d, C: %d, R: %d", left, centre, right);
  serial0_print_string(msg);

  if (distance_values[1] < front_hard_lim) {
    front_hard_lim = FRONT_HARD_LIM_2;
    // motor_turn_spot(turn_bias_manager(distance_values[0], distance_values[2]));
    sprintf(msg, "\nhard %d", (distance_values[0] < distance_values[2]));
    serial0_print_string(msg);
    motor_turn_spot(distance_values[0] < distance_values[2]);
  } else {
    front_hard_lim = FRONT_HARD_LIM_1;
    if (distance_values[1] < front_soft_lim) {
      front_soft_lim = FRONT_SOFT_LIM_2;
      sprintf(msg, "\nsoft %d", (distance_values[0] < distance_values[2]));
      serial0_print_string(msg);
      motor_turn_forward(distance_values[0] < distance_values[2]);
    } else {
      previous_turn_direction = 1;
      front_soft_lim = FRONT_SOFT_LIM_1;
      serial0_print_string("\nstraight");
      motor_straight_forward();
    }
  }
  // the "wall avoidance" below introduces jitter
  // if (distance_values[0] < side_hard_lim) { 
  //   sprintf(msg, "left hard limit\n");
  //   serial0_print_string(msg);
  //   motor_turn_forward(0);
  // } else if (distance_values[2] < side_hard_lim) { 
  //   sprintf(msg, "right hard limit\n");
  //   serial0_print_string(msg);
  //   motor_turn_forward(1);
  // }

  // sprintf(msg, "Speed: %d, %d\nTurning: %d, %d\n", motor_data[0], motor_data[1], motor_data[2], motor_data[3]);
  // serial0_print_string(msg);
  rs_motor_conversion();
}

void mode_6() {

  get_distances(distance_values);
  int left = distance_values[0];
  int centre = distance_values[1];
  int right = distance_values[2];

  sprintf(msg, "\n\nL: %d, C: %d, R: %d", left, centre, right);
  serial0_print_string(msg);

  if (centre < front_hard_lim) {
    sprintf(msg, "\nfront hard %s", (left < right) ? "right" : "left");
    serial0_print_string(msg);
    motor_turn_spot(left < right);
  } else if ( left < side_hard_lim || right < side_hard_lim ) {
    sprintf(msg, "\nside hard %s", (left < right) ? "right" : "left");
    serial0_print_string(msg);
    motor_turn_spot(left > right);
  } else {
    serial0_print_string("\nstraight");
    motor_straight_forward();
  }
  // sprintf(msg, "Speed: %d, %d\nTurning: %d, %d\n", motor_data[0], motor_data[1], motor_data[2], motor_data[3]);
  // serial0_print_string(msg);
  rs_motor_conversion();
}

void mode_7() {

  get_distances(distance_values);
  int left = distance_values[0];
  int centre = distance_values[1];
  int right = distance_values[2];

  sprintf(msg, "\n\nL: %d, C: %d, R: %d", left, centre, right);
  serial0_print_string(msg);

  if ( centre < front_hard_lim ) {
    // about to hit front wall - turn hard in open direction
    motor_turn_spot(turn_bias_manager(left, right));
    sprintf(msg, "\nfront hard %s", (previous_turn_direction) ? "right" : "left");
    serial0_print_string(msg);
  } else if ( centre < front_soft_lim ) {
    // distance before front wall
    if (left < side_hard_lim || right < side_hard_lim) {
      // close to side wall - turn hard in open direction
      motor_turn_spot(turn_bias_manager(left, right));
      sprintf(msg, "\nside hard %s", (previous_turn_direction) ? "right" : "left");
      serial0_print_string(msg);
    } else {
      // not close to side wall - turn soft in open direction - prepare for upcoming wall
      motor_turn_forward(turn_bias_manager(left, right));
      sprintf(msg, "\nfront soft %s", (previous_turn_direction) ? "right" : "left");
      serial0_print_string(msg);
    }
  } else {
    // no front wall
    if (left < side_hard_lim || right < side_hard_lim) {
      // side wall soft turn
      motor_turn_forward(turn_bias_manager(left, right));
      sprintf(msg, "\nside soft %s", (previous_turn_direction) ? "right" : "left");
      serial0_print_string(msg);
    } else {
      // clear and straight
      previous_turn_direction = 1; 
      sprintf(msg, "\nstraight forward");
      serial0_print_string(msg);
      motor_straight_forward();
    }
  }
  rs_motor_conversion();
}

void mode_test() {
  // motor_direct_set(250, 1, 240, 1);
  // motor_turn_forward(0);
  motor_left_forward();
  // motor_right_forward();
  rs_motor_conversion();
}

int turn_bias_manager(uint16_t left_dist, uint16_t right_dist) {
  // left<right
  // 0 for left, 2 for right

  serial0_print_string("\nturn bias manager");
  if ( previous_turn_direction == 0) {
    left_dist = left_dist * turn_bias;
  } else if ( previous_turn_direction == 2 ) {
    right_dist = right_dist * turn_bias;
  }

  if (left_dist < right_dist) { 
    previous_turn_direction = 2; // store result
    return 1; // return direction
  } else {
    previous_turn_direction = 0;
    return 0;
  }
}

void auto_loop() {
  // read beacon
  if (0) {
    // follow beacon
  } else {
    // navigate with sensors
    mode_7();
  }
}