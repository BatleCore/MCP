#include "motor_control.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

void motor_data_conversion(int speed, int turning, uint8_t* results, int* bug)
{
  static float hyst = 0.1;
  static float turning_cap = 0.33;

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
    results[0] = abs((slow_side - centre_BOT) * 255 / centre_BOT);
    results[1] = ((int)slow_side > centre_BOT) ? 2 : ((int)slow_side < centre_BOT) ? 0 : 1;
    // right side faster
    results[2] = abs((fast_side - centre_BOT) * 255 / centre_BOT);
    results[3] = ((int)fast_side > centre_BOT) ? 2 : ((int)fast_side < centre_BOT) ? 0 : 1;
  }
  else
  {
    // left side faster
    results[2] = abs((slow_side - centre_BOT) * 255 / centre_BOT);
    results[3] = ((int)slow_side > centre_BOT) ? 2 : ((int)slow_side < centre_BOT) ? 0 : 1;
    // right side slower
    results[0] = abs((fast_side - centre_BOT) * 255 / centre_BOT);
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