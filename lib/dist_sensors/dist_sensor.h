#ifndef DIST_SENSOR_H_
#define DIST_SENSOR_H_

// === Standard AVR Includes ===
#include "adc.h"
#include "serial.h"

// === Unit Libraries ===

// === Custom Libraries ===
#include "PIN_ASSIGNMENT.h"

// === Constants and Configuration ===
#define DIST_SENSOR_MAX_VAL 600 // max dist in mm
#define DIST_SENSOR_MIN_VAL 70 // min dist in mm
#define DIST_SENSOR_MAX_ERR 9999 // over range
#define DIST_SENSOR_MIN_ERR 0 // under range

// === Global Variables ===

// === Functions ===
void get_distances(uint16_t* distance_values);
void dist_sensor_test();


#endif