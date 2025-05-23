#include "auto_mode.h"

#define FRONT_HARD_LIM_1 125
#define FRONT_HARD_LIM_2 150
#define FRONT_SOFT_LIM_1 200
#define FRONT_SOFT_LIM_2 250
#define SIDE_HARD_LIM 70

uint16_t distance_values[3] = {0}; // initialize all to 0

int front_hard_lim = FRONT_HARD_LIM_1; // hard limit - dont go forward
int front_soft_lim = FRONT_SOFT_LIM_1; // soft limit - begin wide turn
int side_hard_thresh = SIDE_HARD_LIM;
int previous_turn_direction = 1;
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
  if (distance_values[1] < front_hard_lim) {
    front_hard_lim = FRONT_HARD_LIM_2;
    motor_turn_spot(turn_bias_manager(distance_values[0], distance_values[2]));
  } else {
    front_hard_lim = FRONT_HARD_LIM_1;
    if (distance_values[1] < front_soft_lim) {
      front_soft_lim = FRONT_SOFT_LIM_2;
      motor_turn_forward(turn_bias_manager(distance_values[0], distance_values[2]));
    } else {
      previous_turn_direction = 1;
      front_soft_lim = FRONT_SOFT_LIM_1;
      motor_straight_forward();
    }
  }
  // the "wall avoidance" below introduces jitter
  if (distance_values[0] < side_hard_thresh) { 
    sprintf(msg, "left hard limit\n");
    serial0_print_string(msg);
    motor_turn_forward(0);
  } else if (distance_values[2] < side_hard_thresh) { 
    sprintf(msg, "right hard limit\n");
    serial0_print_string(msg);
    motor_turn_forward(1);
  }

  // sprintf(msg, "Speed: %d, %d\nTurning: %d, %d\n", motor_data[0], motor_data[1], motor_data[2], motor_data[3]);
  // serial0_print_string(msg);
  
  rs_motor_conversion();

}

int turn_bias_manager(uint8_t left_dist, uint8_t right_dist) {
  // 0 for left, 2 for right
  if ( left_dist * ((previous_turn_direction == 0) ? 1.1 : 1) > right_dist  * ((previous_turn_direction == 2) ? 1.1 : 1)) {
    previous_turn_direction = 0;
    return 0;
  } else {
    previous_turn_direction = 2;
    return 1;
  }
}

void auto_loop() {
  if (0) {
    // follow beacon
  } else {
    mode_5();
  }
}