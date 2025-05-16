#include "battery.h"
#include "motor_control.h"
#include "servo_control.h"
#include "dist_sensor.h"

void init_everything() {
  // Provided Libraries
  adc_init();
  serial0_init();
  serial1_init();
  serial2_init();
  serial4_init();
  serial5_init();
  lcd_init();
  // Our Libraries
  motor_init();
  servo_init();
}

int main(void) {
  motor_init();
  servo_init();

  dist_sensor_test();

  return 0;
}