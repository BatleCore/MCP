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

//constants



//functions
void mode_1();
void mode_2();
void mode_3();
void mode_4();
void mode_5();
void motor_stop();
void motor_straight_forward();
void motor_left_forward();
void motor_right_forward();
void motor_turn_forward(int turn_dir);
void motor_turn_spot(int turn_dir);
void motor_turn_modifier(int turn_dir);


#endif