#ifndef DIST_SENSOR_H_
#define DIST_SENSOR_H_

#include "PIN_ASSIGNMENT.h"
#include "adc.h"
#include "serial.h"

#define DIST_SENSOR_MAX_VAL 600// value in mm
#define DIST_SENSOR_MIN_VAL 70
#define DIST_SENSOR_MAX_ERR 9999
#define DIST_SENSOR_MIN_ERR 0

uint16_t dist_measurement(int PIN);
void get_distances(uint16_t* distance_values);
void dist_sensor_test();
void dist_sensor_RawDataFeed(int PIN);


#endif