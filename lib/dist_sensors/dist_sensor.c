#include "dist_sensor.h"

void get_distances(uint16_t* distance_values) {
  /*
  [0] - LEFT
  [1] - CENTRE
  [2] - RIGHT
  */

  uint16_t left_sum = 0;
  uint16_t cent_sum = 0;
  uint16_t right_sum = 0;
  int samples = 50;

  // take 10 measurements for each sensor
  for(int i=0; i<samples; i++) {
    left_sum = left_sum + adc_read(PIN_DISTANCE_LEFT);
    cent_sum = cent_sum + adc_read(PIN_DISTANCE_CENT);
    right_sum = right_sum + adc_read(PIN_DISTANCE_RIGHT);
    _delay_us(10);
  }
  
  // read and convert
  distance_values[0] = 28900.0/((left_sum / samples)+10) - 17;  // left calibration
  distance_values[1] = 28600.0/((cent_sum / samples)+11) - 13;  // centre calibration
  distance_values[2] = 29400.0/((right_sum/ samples)+13) - 23;  // right calibration

  // clamp for valid range
  distance_values[0] = (distance_values[0] > DIST_SENSOR_MAX_VAL) ? DIST_SENSOR_MAX_ERR : (distance_values[0] < DIST_SENSOR_MIN_VAL) ? DIST_SENSOR_MIN_ERR : distance_values[0];
  distance_values[1] = (distance_values[1] > DIST_SENSOR_MAX_VAL) ? DIST_SENSOR_MAX_ERR : (distance_values[1] < DIST_SENSOR_MIN_VAL) ? DIST_SENSOR_MIN_ERR : distance_values[1];
  distance_values[2] = (distance_values[2] > DIST_SENSOR_MAX_VAL) ? DIST_SENSOR_MAX_ERR : (distance_values[2] < DIST_SENSOR_MIN_VAL) ? DIST_SENSOR_MIN_ERR : distance_values[2];
}

// Debugging Function - Prints out raw ADC values
void dist_sensor_test() {
  serial0_init();
  adc_init();
  _delay_ms(20);

  uint16_t dists[3];
  char msg[40];

  while ( 1 ) {
    get_distances(dists);
    sprintf(msg, "\n\nLeft:  %d\nCent:  %d\nRight:  %d", dists[0], dists[1], dists[2]);
    serial0_print_string(msg);
    _delay_ms(200);

  }
}
