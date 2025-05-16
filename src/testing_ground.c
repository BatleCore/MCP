#include "battery.h"
#include "motor_control.h"
#include "servo_control.h"
#include "dist_sensor.h"
#include "light_sensor.h"

int main(void) {
  motor_init();
  servo_init();

  // dist_sensor_test();
  // uint8_t motor_data[4];
  
  // motor_test();
  // motor_data[0] = 250;
  // motor_data[1] = 2;
  // motor_data[2] = 250;
  // motor_data[3] = 2;
  // differential_PWM_v3(motor_data);

  LDR_test();

  return 0;
}