#include "test_battery.h"
#include "motor_control.h"
#include "servo_control.h"

int main(void) {
  motor_init();
  servo_init();

  // send_ADC_to_serial();


  // motor_test();
  uint16_t pos = 1000;
  servo_set_pos(pos);
  _delay_ms(1000);
  pos = 2000;
  servo_set_pos(pos);

  return 0;
}