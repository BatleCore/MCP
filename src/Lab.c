/*
**TASK 1**

Your task is to achieve 2-way simultaneous communication between 2 microcontrollers. 
- Microcontroller one "*robot*" will read and transmit sensor values 
  (range sensors/ light sensors), and recieve commands to control a servo motor.
- Microcontroller two "*controller*" will read and transmit joystick values while 
  printing relevant sensor values from serial to an LCD screen or terminal.
- You will need to interpret the signals and display on the *controller* if the robot 
  should be stationary, moving forward, left or right based on light dependant resistor 
  sensing.

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
#include "Controller.h"
#include "serial.h"
#include "milliseconds.h"
#include "hd44780.h"

/*
## LCD

16 characters per line
lcd_goto( 0x40 ); << hexadecimal val for 64, sends to first char of line 2

*/

int main(void)
{
  serial2_init(); // sends to robot via serial 2
  milliseconds_init();
  adc_init();
  _delay_ms(10); // allow ADC to initialize

  // analog data variables
  volatile uint16_t JOY_X_val, JOY_Y_val;
  uint8_t JOY_X_pin = 0; // pin to be allocated
  uint8_t JOY_Y_pin = 0; // pin to be allocated
  uint8_t instruction[2];

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
      JOY_X_val = adc_read(JOY_X_pin); // currently 16 bit
      JOY_Y_val = adc_read(JOY_Y_pin); // currently 16 bit

      instruction_for_robot(JOY_X_val, JOY_Y_val); // defines data for "instruction"


      serial2_write_bytes(2, instruction[0], instruction[2]); // 1 byte = 8 bits
      last_send_ms = current_ms;
    }

    // Receive Serial
    if (serial2_available()) // only for receiving data
    {
      serial2_get_data(recievedData, 2); // data variable, data size (bytes)

      // proccess data, how should we move?
      // convert to message for LCD screen

      /*******************
      ** 100%L    100%R **
      **      STOP      **
      **  FORWARD LEFT  **
      **  FORWARD RIGHT **
      **FORWARD STRAIGHT**
      **  REVERSE LEFT  **
      **  REVERSE RIGHT **
      **REVERSE STRAIGHT**
      *******************/

      // send to LCD

      // sprintf(serial_string, "\nData1: %3u, Data2: %3u", recievedData[0], recievedData[1]);
      // serial0_print_string(serial_string);
    }
  }
  return(1);
}//end main 

uint8_t instruction_for_robot(uint8_t X_val, uint8_t y_val){
  // what means what?
  // X_val controls motor left/right
  // Y_val controls motor speed
  // instruction[0] for robot speed ( 0 for stop )
  // instruction[1] for direction (0 for left, 255 for right, vary in between)

  return(0);
}

void instructions_from_data(void){

}