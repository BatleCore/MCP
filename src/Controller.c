
// CONTROLLER
// Lab 8 Controller Micro
#include "Controller.h"
#include "motor_control.h"


// Joystick Analog Pins (forgot how this works on controller)
/*
Lx = A14 = PK6
Ly = A15 = PK7
Rx = A1 = PF1
Ry = A0 = PF0
*/

#define PIN_JOYSTICKLEFT_X PK6
#define PIN_JOYSTICKLEFT_Y PK7
#define PIN_JOYSTICKRIGHT_X PF1
#define PIN_JOYSTICKRIGHT_Y PF0

#define PORT_JOYLEFT PORTK
#define DDR_JOYLEFT DDRK
#define PORT_JOYRIGHT PORTF
#define DDR_JOYRIGHT DDRF

// Communication Codes (matches the robot)
#define LDR_REQUEST  0xA0  // Request light sensor data from robot
#define JOYSTICK_MOTOR_READ  0xA1  // Send joystick X/Y to robot for motor control
#define JOYSTICK_SERVO_READ  0xA2  // Send joystick X/Y to robot for servo control
#define REQUEST_ERROR  0xEE  // Error / fallback code (not used here)

// Convert 10-bit ADC (0–1023) to 8-bit value (0–255)
uint8_t mapADC(uint16_t adc_val) {
  /******************************
    Not used
    made redundant by "motor_data_conversion" 
  ******************************/
  return adc_val >> 2; // divide by 4
}

// System setup
// - Initializes communication (serial2 for Xbee)
// - Initializes ADC and millisecond timing
void setup() {
  cli();           // Disable interrupts while setting up
  serial2_init();      // XBee serial communication
  serial0_init();      // Teminal Serial Monitor
  milliseconds_init();   // For timing control
  // ADC PINS SET TO INPUT
  DDR_JOYLEFT &= ~(1<<PIN_JOYSTICKLEFT_X);
  DDR_JOYLEFT &= ~(1<<PIN_JOYSTICKLEFT_Y);
  DDR_JOYRIGHT &= ~(1<<PIN_JOYSTICKRIGHT_X);
  DDR_JOYRIGHT &= ~(1<<PIN_JOYSTICKRIGHT_Y);
  _delay_ms(20);
  adc_init();        // Enable analog input (for joysticks)
  _delay_ms(20);
  sei();           // Enable global interrupts
}
 
// Main control loop
// Reads joystick values and sends them to the robot every 20ms
int main(void) {
  setup();  // Init everything

  int left_x_val;   // Raw ADC reading for joystick X
  int left_y_val;   // Raw ADC reading for joystick Y
  uint32_t lastSend = 0;   // Last time a packet was sent
  uint32_t lastSend_debug = 0;   // Last time a packet was sent

  char msg[30]; // serial string
  uint8_t motor_d[4]; // converted motor data
  motor_d[0] = 0;
  motor_d[2] = 0;
  int debug_data[5]; // debug data from "motor_data_conversion"
  uint8_t servo_d[2]; // converted servo data
  servo_d[0] = 0;
  servo_d[1] = 0;

  while (1) {
    // Check if 20ms has passed since last send
    // This limits packet rate to 50Hz (fast, but not overwhelming)
    if (milliseconds_now() - lastSend >= 20) {
      lastSend = milliseconds_now();

      // Read left joystick (X = turn, Y = forward/backward)
      // left_x_val = adc_read(PIN_JOYSTICKRIGHT_X);
      // left_y_val = adc_read(PIN_JOYSTICKRIGHT_Y); 
      // left_y_val = adc_read(PIN_JOYSTICKLEFT_Y); 
 
      // sprintf(msg, "\nNEW\nJoy X: %d\nJoy Y: %d\n", left_x_val, left_y_val);
      // serial0_print_string(msg);
      
      motor_data_conversion(motor_d, debug_data);
      
      // Send joystick data as a 5-byte command packet:
      serial2_write_bytes(5, JOYSTICK_MOTOR_READ, motor_d[0], motor_d[1], motor_d[2], motor_d[3]);

      
      servo_read_joystick(servo_d);
      serial2_write_bytes(3, JOYSTICK_SERVO_READ, servo_d[0], servo_d[1]);

    }
    if (milliseconds_now() - lastSend_debug >= 200) {
      lastSend_debug = milliseconds_now();
      
      sprintf(msg, "\nvel: %d, dir: %d", servo_d[0], servo_d[1]);
      serial0_print_string(msg);
    //   // debugging - print to terminal
    //   sprintf(msg, "L: %d : %d\n", motor_d[1], motor_d[0]);
    //   serial0_print_string(msg);
    //   sprintf(msg, "R: %d : %d\n", motor_d[3], motor_d[2]);
    //   serial0_print_string(msg);
    }
  }
  return 0;
}