#include "auto_mode.h"



uint16_t distance_values[3] = {0}; // initialize all to 0
// uint8_t motor_data[4] = {0};
int front_hard_thresh = 125; // hard limit - dont go forward
int front_soft_thresh = 200; // soft limit - begin wide turn
int side_hard_thresh = 150;
char msg[50];

void auto_init() {
  // set servo to 'open'
  servo_set_pos(SERVO_PULSE_OPEN);
}

void mode_5() {

  get_distances(distance_values);

  if (distance_values[1] < front_hard_thresh) { 
    sprintf(msg, "front hard limit\n");
    serial0_print_string(msg);
    motor_turn_spot((distance_values[2]<distance_values[0]));
  } else if (distance_values[1] < front_soft_thresh) {
    sprintf(msg, "front soft limit\n");
    serial0_print_string(msg);
    motor_turn_forward((distance_values[2]<distance_values[0]));
  } else {
    motor_straight_forward();
  }

  // if (distance_values[0] < side_hard_thresh) { 
  //   sprintf(msg, "left hard limit\n");
  //   serial0_print_string(msg);
  //   motor_turn_modifier(0);
  // }
  // if (distance_values[1] < side_hard_thresh) { 
  //   sprintf(msg, "right hard limit\n");
  //   serial0_print_string(msg);
  //   motor_turn_modifier(1);
  // }

  // sprintf(msg, "Speed: %d, %d\nTurning: %d, %d\n", motor_data[0], motor_data[1], motor_data[2], motor_data[3]);
  // serial0_print_string(msg);
  
  rs_motor_conversion();

}

void auto_loop() {
  if (0) {
    // follow beacon
  } else {
    mode_5();
  }
}