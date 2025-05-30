#ifndef AUTO_MODE_H_
#define AUTO_MODE_H_

//include standard libraries
#include "adc.h"
#include "serial.h"

//include provided header files

//include custom header files
#include "PIN_ASSIGNMENT.h"
#include "dist_sensor.h"
#include "motor_control.h"
#include "servo_control.h"
#include "light_sensor.h"
#include "communication.h" // probably wont stay here

//constants

//functions
void auto_init();
void mode_5();
void mode_6();
void mode_7();
void mode_test();
void auto_loop();
int turn_bias_manager(uint16_t left_dist, uint16_t right_dist);

#ifndef CURRENT_MODE
#define CURRENT_MODE
uint8_t current_mode;
#endif

#endif