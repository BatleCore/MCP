#include "test_battery.h"
#include "motor_control.h"
#include "servo_control.h"
#include "dist_sensor.h"

int main(void) {
  motor_init();
  servo_init();

  dist_sensor_test();

  return 0;
}