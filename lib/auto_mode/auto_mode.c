#include "auto_mode.h"

#define MOTOR_MAX 250
#define MOTOR_AUTO_SPEED 250
#define MOTOR_AUTO_TURN 250

uint16_t distance_values[3] = {0}; // initialize all to 0
uint8_t motor_data[4] = {0};
int front_hard_thresh = 125; // hard limit - dont go forward
int front_soft_thresh = 200; // soft limit - begin wide turn
int side_hard_thresh = 150;
char msg[50];

void mode_1() {
  // go straight
  // stop at wall
  get_distances(distance_values);
  char msg[35];
  sprintf(msg, "\nL: %d\nC: %d\nR: %d\n", distance_values[0], distance_values[1], distance_values[2]);
  serial0_print_string(msg);
  if (distance_values[1]> 100) { // move forward
    motor_data[0] = MOTOR_MAX;
    motor_data[1] = 1; // forward, always forward
    motor_data[2] = 0; // straight
    motor_data[3] = 1; // right (not significant)
  } else { // stop
    sprintf(msg, "STOP\n");
    serial0_print_string(msg);
    motor_stop();
  }
  // sprinf(msg, "%d", distance_values[1]);
  // serial0_print_string(msg);
  rs_motor_conversion(motor_data);
}

void mode_2() {
  // stay between walls, back out of corners
  // much work needed
  char msg[35];
  float turn_r; // turn ratio, the imbalance of L/R distances, the correction required
  float L;
  float R;
  // go straight
  // walls at equal distance
  get_distances(distance_values);
  sprintf(msg, "\nL: %d\nC: %d\nR: %d\n", distance_values[0], distance_values[1], distance_values[2]);
  serial0_print_string(msg);
  L = (float)distance_values[0];
  R = (float)distance_values[2];
  if (distance_values[1] > front_hard_thresh) { // && distance_values[0] != DIST_SENSOR_MAX_ERR && distance_values[2] != DIST_SENSOR_MAX_ERR
    front_hard_thresh = 100;
    if (distance_values[0] > distance_values[2]) { // need to adjust left
      turn_r = (L - R) / (L + R);
      motor_data[0] = MOTOR_MAX;  // speed magnitude inverse-proportional to imballance
      motor_data[1] = 1; // forward, always forward
      motor_data[2] = 150; // turn magnitude proportional to imballance
      motor_data[3] = 0; // left
      // sprintf(msg, "LEFT %d\n", motor_data[2]);
      // serial0_print_string(msg);
    } else { // need to adjust right
      turn_r = (R - L) / (L + R);
      motor_data[0] = MOTOR_MAX;  // speed magnitude inverse-proportional to imballance
      motor_data[1] = 1; // forward, always forward
      motor_data[2] = 150; // turn magnitude proportional to imballance
      motor_data[3] = 1; // right
      // sprintf(msg, "RIGHT %.2f\n", turn_r);
      // serial0_print_string(msg);
    }
  } else { // stop
    // sprintf(msg, "STOP\n");
    // serial0_print_string(msg);
    motor_data[0] = MOTOR_MAX;
    motor_data[1] = 0;
    motor_data[2] = 250;
    front_hard_thresh = 125;
    // motor_data[3] = !motor_data[3];
      // motor_stop();
  }
  rs_motor_conversion(motor_data);
}

void mode_3(){
  // identify wall dropping away
  motor_straight_forward();
  rs_motor_conversion(motor_data);
}

void mode_4() {
  /* HUG WALL
  - chooses the closest wall by default
  - measure  distance to wall
  - continue straight
  - if distance increases, turn towards wall
  - if distance decreases, turn away from 

  */
  static char msg[30];
  static int wall_preference = 1; // 0 left, 1 none, 2 right
  static int wall_distance_reference;
  static int wall_allowance = 20; // mm
  get_distances(distance_values);
  if (wall_preference == 1) { // no preference yet
    wall_preference = (distance_values[0] > distance_values[2]) * 2; // 2 if left bigger, follow right
    sprintf(msg, "wall preference: %d", wall_preference);
    serial0_print_string(msg);
    wall_distance_reference = distance_values[wall_preference];
    motor_straight_forward();
  } else { // follow that wall
    motor_straight_forward();
    if ( distance_values[wall_preference] > wall_distance_reference + wall_allowance ) { // distance increase
      motor_turn_forward(wall_preference);
    } else if ( distance_values[wall_preference]  + wall_allowance > wall_distance_reference ) { // distance decrease
      motor_turn_forward(!wall_preference);
    }
  }
  // motor_data[0] = MOTOR_AUTO_SPEED;
  // motor_data[1] = 1; // forward, always forward
  // motor_data[2] = 0; // straight
  // motor_data[3] = 1; // right (not significant)
  // motor_straight_forward();
  sprintf(msg, "Speed: %d: %d\nTurn: %d: %d\n\n", motor_data[0], motor_data[1], motor_data[2], motor_data[3]);
  serial0_print_string(msg);
  rs_motor_conversion(motor_data);
}

void mode_5() {
  /*
  
  */
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
  sprintf(msg, "Speed: %d, %d\nTurning: %d, %d\n", motor_data[0], motor_data[1], motor_data[2], motor_data[3]);
  serial0_print_string(msg);
  rs_motor_conversion(motor_data);

}

void motor_stop(){
  motor_data[0] = 0;
  motor_data[1] = 1; // forward, always forward
  motor_data[2] = 0; // straight
  motor_data[3] = 0; // left  (not significant)
}

void motor_straight_forward(){
  motor_data[0] = MOTOR_AUTO_SPEED;
  motor_data[1] = 1; // forward, always forward
  motor_data[2] = 0; // straight
  motor_data[3] = 0; // left  (not significant)
}

void motor_left_forward(){
  motor_data[0] = MOTOR_AUTO_SPEED;
  motor_data[1] = 1; // forward, always forward
  motor_data[2] = MOTOR_AUTO_TURN;
  motor_data[3] = 0; // left
}

void motor_right_forward(){
  motor_data[0] = MOTOR_AUTO_SPEED;
  motor_data[1] = 1; // forward, always forward
  motor_data[2] = MOTOR_AUTO_TURN;
  motor_data[3] = 1; // right
}

void motor_turn_forward(int turn_dir){
  motor_data[0] = MOTOR_AUTO_SPEED;
  motor_data[1] = 1; // forward, always forward
  motor_data[2] = MOTOR_AUTO_TURN;
  motor_data[3] = turn_dir; // passed as arg, 0 or 1
}

void motor_turn_spot(int turn_dir){
  motor_data[0] = 0;
  motor_data[1] = 1; // forward, always forward
  motor_data[2] = MOTOR_AUTO_TURN;
  motor_data[3] = turn_dir; // passed as arg, 0 or 1
}

void motor_turn_modifier(int turn_dir) { 
  int temp_value;
  if (motor_data[3]) {
    temp_value = 250 + motor_data[2];
  }
  else {
    temp_value = 250 - motor_data[2];
  }
  temp_value += 100 * (-1 + turn_dir * 2);

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