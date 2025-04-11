/*
**TASK 1**

Your task is to achieve 2-way simultaneous communication between 2 microcontrollers. 
- Microcontroller one "*robot*" will read and transmit sensor values 
  (range sensors/ light sensors), and recieve commands to control a servo motor.
- Microcontroller two "*controller*" will read and transmit joystick values while 
  printing relevant sensor values from serial to an LCD screen or terminal.
- You will need to interpret the signals and display on the *controller* if the robot 
  should be stationary, moving forward, left, or right based on light dependant 
  resistor sensing.

### Outcomes

1. Define the communication protocol for serial communication 
   between the robot and controller microcontrollers.
	  - Ensure it is clear how the instructions are structured and how they are delimited.
	  - Make it clear what information is sent in your communication protocol.
 		- Define this separately for each direction
2. Discuss design considerations for reliable commmunications:
   	- What is buffer overflow and why would it occur.
   	- What error detection exists in this communication. What could be added?
3. Demonstrate functional 1-way communication*
4. Demonstrate functional 2-way communication*
	  - Including use of LCD screen
5. Demonstrate a functional interpretation of the sensor data
	  - This must include some data transformation to make it readable, 
    consider what might be useful for the final project.
*/

//Example ATmega2560 Project
//File: ATmega2560Project.c
//An example file for second year mechatronics project

//include this .c file's header file
#include "Robot.h"
#include "serial.h"
#include "milliseconds.h"

//static function prototypes, functions only called in this file

/*
- read ADC values
- send ADC values via serial to controller
- receive instructions from controller
- follow instructions

if ( time )

*/

int main(void)
{
  serial2_init(); // sends to controller via serial 2
  milliseconds_init();
  adc_init();
  _delay_ms(10); // allow ADC to initialize

  // analog data variables
  volatile uint16_t LDR_left_val, LDR_right_val; // from 0 to 1023 ( 10 bit? )
  uint8_t LDR_left_pin = 0; // pin to be allocated
  uint8_t LDR_right_pin = 0; // pin to be allocated

  // serial data vairables
  // how many bytes are we sending?
  uint32_t current_ms, last_send_ms;
  uint8_t databyte1 = 0;
  uint8_t databyte2 = 0;
  uint8_t recievedData[2]; // only 2?
  char serial_string[60] = {0};

  while(1)
  {
    current_ms = milliseconds_now();

    // Send Serial
    if( (current_ms-last_send_ms) >=  100 ) // if sample interval large enough
    {

      // read ADC values
      LDR_left_val = ADC_valConvert(adc_read(LDR_left_pin)); // currently 16 bit
      LDR_right_val = ADC_valConvert(adc_read(LDR_right_pin)); // currently 16 bit

      // update dataBytes
      // this will need to be adjusted.
      // either data shrunk or bytes split and recombined
      serial2_write_bytes(2, databyte1, databyte2); // 1 byte = 8 bits
      last_send_ms = current_ms;
    }

    // Receive Serial
    if (serial2_available()) // only for receiving data
    {
      // "receivedData" is the instruction from controller
      // 2 bits: 
      //        [0] for speed (0 to 255) 
      //        [0] for direction (0 left, 127 straight, 255 right, gradient in between)
      serial2_get_data(recievedData, 2); // data variable, data size (bytes)
      move_robot(recievedData[0], recievedData[1]);
    }
  }
  return(1);
}

uint16_t ADC_valConvert(uint16_t val){
  uint16_t value = val; // place-holder
  return(value);
}

void move_robot(uint8_t speed, uint8_t direction){
  // this will probably have to change. 
  // robot control is manual, not by sensor logic
  // data will probaby be speed for motor_left and motor_right

  if(speed){
    //speedy things
  }
  else
  {
    // halt all motion
    // set motors to 0
  }
}