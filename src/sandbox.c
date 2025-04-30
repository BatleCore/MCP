#include "motor_control.h"
#include <stdio.h>

int main()
{
  int speed = 100;
  int turning = 700;
  
  int* motor_d = motor_data(speed, turning);

  // printf("left_dir: %d\tleft_duty: %d", motor_d[0], motor_d[1]);
  printf("test");
  return 0;
}