#include "motor_control.h"  // Include the header for motor control functions
#include <stdio.h>           // Include the standard I/O library for printf

int main() {
    // Initialize the speed and turning values
    int speed = 965;       // Example speed value
    int turning = 125;     // Example turning value
    
    // Print a debug message to ensure the program starts
    printf("\n\n\nProgram started\n");
    // printf("val: %d\n", speed);

    // Call motor_data() to get the motor direction and duty cycle
    int* motor_d = motor_data(speed, turning);

    // Print the direction and duty cycle values
    printf("left_dir: %d\tleft_duty: %d\n", motor_d[1], motor_d[0]);
    printf("right_dir: %d\tright_duty: %d\n\n", motor_d[3], motor_d[2]);


    return 0;  // Return 0 to indicate successful execution
}