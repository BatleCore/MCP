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


#define BAUDRATE 38400 // baud rate

#define F_OSC 16000000 // micro clock speed

#define packet_size 8  // packet size for communication

/*
## LCD

16 characters per line
lcd_goto( 0x40 ); << hexadecimal val for 64, sends to first char of line 2

*/

int main(void)
{
  milliseconds_init();
  adc_init();
  _delay_ms(10); // allow ADC to initialize
  lcd_init();
  serial0_init();
  serial2_init();


  // analog data variables
  volatile uint16_t JOY_X_val, JOY_Y_val;
  uint8_t x_mapped, y_mapped;
  uint8_t JOY_X_pin = PF0; // pin to be allocated
  uint8_t JOY_Y_pin = PF1; // pin to be allocated
  uint8_t recievedData[2];

  volatile uint8_t LDR_L = 0;
  volatile uint8_t LDR_R = 0;

  // serial data vairables
  // how many bytes are we sending?
  uint32_t current_ms, last_send_ms;
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

      // serial2_write_bytes(2, instruction[0], instruction[2]); // 1 byte = 8 bits
      sprintf(serial_string, "\nX: %d, Y: %d", JOY_X_val, JOY_Y_val);
      serial0_print_string(serial_string);
      
      // instruction_for_robot(JOY_X_val, JOY_Y_val); // defines data for "instruction"
      // TX
      x_mapped = (JOY_X_val>>2);
      y_mapped = (JOY_Y_pin>>2);
      serial2_write_bytes(2, x_mapped, y_mapped); 
      // LCD
      LDR_L = recievedData[0];
      LDR_R = recievedData[1];
      print_to_lcd("LDR data", LDR_L, LDR_R);
      // print_to_lcd("joystick data", JOY_X_val, JOY_Y_val);
      last_send_ms = current_ms;
    }

    /*
    // Receive Serial
    if (serial2_available()) // only for receiving data
    {
      serial2_get_data(recievedData, 2); // data variable, data size (bytes)

      // proccess data, how should we move?
      // convert to message for LCD screen

      ********************
      ** 100%L    100%R **
      **      STOP      **
      **  FORWARD LEFT  **
      **  FORWARD RIGHT **
      **FORWARD STRAIGHT**
      **  REVERSE LEFT  **
      **  REVERSE RIGHT **
      **REVERSE STRAIGHT**
      ********************

      // send to LCD

      // sprintf(serial_string, "\nData1: %3u, Data2: %3u", recievedData[0], recievedData[1]);
      // serial0_print_string(serial_string);
    }
    */

    if( serial2_available() )
    {
      //Function takes the array to return data to and the number of bytes to be read.
      serial2_get_data(recievedData, 2); 
      sprintf(serial_string,"\nData 1: %3u, Data2: %3u", recievedData[0],recievedData[1]); //Format string
      serial0_print_string(serial_string); //Print string to usb serial
    
    }

  }
  return(1);
}

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

void print_to_lcd(char * msg1, int16_t val1, int16_t val2)
{
  char LCD_msg[17] = {0};
  lcd_clrscr();
  lcd_goto(0);
  snprintf(LCD_msg, sizeof(LCD_msg), "X: %d, Y: %d", val1, val2);
  lcd_puts(LCD_msg);
  lcd_goto(0x40);
  snprintf(LCD_msg, sizeof(LCD_msg), "%s", msg1);
  lcd_puts(LCD_msg);
}
