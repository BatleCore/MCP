#include "auto_mode.h"

#define MOTOR_MAX 150
uint16_t distance_values[3] = {0}; // initialize all to 0
uint8_t motor_data[4] = {0};
int front_thresh;
// motor_data[0]: speed magnitude
// motor_data[1]: speed direction
// motor_data[2]: turning magnitude
// motor_data[3]: turning direction



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
  rs_motor_conversion(motor_data);
}

void mode_2() {
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
  if (distance_values[1] > 100) { // && distance_values[0] != DIST_SENSOR_MAX_ERR && distance_values[2] != DIST_SENSOR_MAX_ERR
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
    sprintf(msg, "STOP\n");
    serial0_print_string(msg);
    motor_data[0] = 100;
    motor_data[1] = 0;
    motor_data[3] = !motor_data[3];
      // motor_stop();
  }
  rs_motor_conversion(motor_data);
}

void mode_3(){
  // identify wall dropping away
}

void mode_4() {
  // hug wall
}

void motor_stop(){
  motor_data[0] = 0;
  motor_data[1] = 1; // forward, always forward
  motor_data[2] = 0; // straight
  motor_data[3] = 0; // left  (not significant)
  // rs_motor_conversion(motor_data); // extra call - a fail-safe
}