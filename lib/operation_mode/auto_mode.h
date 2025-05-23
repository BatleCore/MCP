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

//constants

//functions
void auto_init();
void mode_5();
void auto_loop();


#endif