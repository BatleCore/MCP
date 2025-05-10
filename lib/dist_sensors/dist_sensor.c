#include "dist_sensor.h"

int dist_measurement(int PIN) {
  // read the pin
  int adc_val = adc_read(PIN);
  // calculate the distance
  int dist_val = 24000.0/adc_val; // prototype code - happy to change

  return dist_val;
}

void dist_sensor_test(int pin) {
  char msg[20];
  int distance_value;
  serial0_init();
  adc_init();
  _delay_ms(20);

  while (1) {
    distance_value = dist_measurement(pin);
    sprintf(msg, "\n%d mm\n", distance_value);
    serial0_print_string(msg);
    _delay_ms(400);
  }
  
}