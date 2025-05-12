#include "dist_sensor.h"

uint16_t dist_measurement(int PIN) {
  /*
  INCOMPLETE
  - CALLIBRATION (UNIQUE TO EACH SENSOR?)
  - MAX AND MIN LIMITS
  */
  uint16_t dist_val = 24000.0/adc_read(PIN); // prototype code - happy to change
  if(dist_val > DIST_SENSOR_MAX_VAL) {
    dist_val = DIST_SENSOR_MAX_ERR;
    // TOO FAR
  } else if (dist_val < DIST_SENSOR_MIN_VAL) {
    // TOO CLOSE
    dist_val = DIST_SENSOR_MIN_ERR;
  }
  return dist_val;
}

void get_distances(uint16_t* distance_values) {
  /*
  [0] - LEFT
  [1] - CENTRE
  [2] - RIGHT
  */

  uint16_t left_sum = 0;
  uint16_t cent_sum = 0;
  uint16_t right_sum = 0;
  int samples = 3;

  for(int i=1; i<samples; i++) {
    left_sum = left_sum + adc_read(PIN_DISTANCE_LEFT);
    cent_sum = cent_sum + adc_read(PIN_DISTANCE_CENT);
    right_sum = right_sum + adc_read(PIN_DISTANCE_RIGHT);
    _delay_us(100);
  }
  
  // read and convert
  distance_values[0] = 28900.0/((left_sum / samples)+10) - 17;
  distance_values[1] = 28600.0/((cent_sum / samples)+11) - 13;
  distance_values[2] = 29400.0/((right_sum/ samples)+13) - 23;

  // clamp for valid range
  distance_values[0] = (distance_values[0] > DIST_SENSOR_MAX_VAL) ? DIST_SENSOR_MAX_ERR : (distance_values[0] < DIST_SENSOR_MIN_VAL) ? DIST_SENSOR_MIN_ERR : distance_values[0];
  distance_values[1] = (distance_values[1] > DIST_SENSOR_MAX_VAL) ? DIST_SENSOR_MAX_ERR : (distance_values[1] < DIST_SENSOR_MIN_VAL) ? DIST_SENSOR_MIN_ERR : distance_values[1];
  distance_values[2] = (distance_values[2] > DIST_SENSOR_MAX_VAL) ? DIST_SENSOR_MAX_ERR : (distance_values[2] < DIST_SENSOR_MIN_VAL) ? DIST_SENSOR_MIN_ERR : distance_values[2];
}


void dist_sensor_test() {
  serial0_init();
  adc_init();
  _delay_ms(20);

  uint16_t dists[3];
  char msg[20];

  while ( 1 ) {
    get_distances(dists);
    sprintf(msg, "\n%d, %d, %d\n", dists[0], dists[1], dists[2]);
    serial0_print_string(msg);
    _delay_ms(100);

  }
}

void dist_sensor_RawDataFeed(int PIN) {
  char msg[20];
  int ADC_value;
  serial0_init();
  adc_init();
  _delay_ms(20);

  while (1) {
    ADC_value = adc_read(PIN);
    sprintf(msg, "\n%d\n", ADC_value);
    serial0_print_string(msg);
    _delay_ms(100);
  }
}