#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void motor_data_conversion(int speed, int turning, uint8_t* results, int* bug);

#endif