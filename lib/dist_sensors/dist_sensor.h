#ifndef DIST_SENSOR_H_
#define DIST_SENSOR_H_

//include standard libraries
#include "adc.h"
#include "serial.h"

//include provided header files

//include custom header files
#include "PIN_ASSIGNMENT.h"

//constants
#define DIST_SENSOR_MAX_VAL 600 // max dist in mm
#define DIST_SENSOR_MIN_VAL 70 // min dist in mm
#define DIST_SENSOR_MAX_ERR 9999 // over range
#define DIST_SENSOR_MIN_ERR 0 // under range

uint16_t dist_measurement(int PIN);
void get_distances(uint16_t* distance_values);
void dist_sensor_test();
void dist_sensor_RawDataFeed(int PIN);


#endif