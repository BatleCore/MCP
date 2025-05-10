#ifndef DIST_SENSOR_H_
#define DIST_SENSOR_H_

#include "PIN_ASSIGNMENT.h"
#include "adc.h"
#include "serial.h"

int dist_measurement(int PIN);
void dist_sensor_test(int pin);


#endif