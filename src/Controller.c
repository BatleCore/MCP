
// CONTROLLER
// Lab 8 Controller Micro
#include "Controller.h"


// Joystick Analog Pins (forgot how this works on controller)
/*
Lx = A14 = PK6
Ly = A15 = PK7
Rx = A1 = PF1
Ry = A0 = PF0
*/

// #define PORT_JOYLEFT PORTK
// #define DDR_JOYLEFT DDRK
// #define PORT_JOYRIGHT PORTF
// #define DDR_JOYRIGHT DDRF

// Communication Codes (matches the robot)
// #define LDR_REQUEST  0xA0  // Request light sensor data from robot
// #define JOYSTICK_MOTOR_READ  0xA1  // Send joystick X/Y to robot for motor control
// #define JOYSTICK_SERVO_READ  0xA2  // Send joystick X/Y to robot for servo control
// #define REQUEST_ERROR  0xEE  // Error / fallback code (not used here)

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
  display_init();
  motor_init_params();

  // ADC PINS SET TO INPUT
  DDR_JOYLEFT &= ~(1<<PIN_JOY_L_X);
  DDR_JOYLEFT &= ~(1<<PIN_JOY_L_Y);
  DDR_JOYRIGHT &= ~(1<<PIN_JOY_R_X);
  DDR_JOYRIGHT &= ~(1<<PIN_JOY_R_X);
  _delay_ms(20);
  adc_init();        // Enable analog input (for joysticks)
  _delay_ms(20);
  sei();           // Enable global interrupts
}
 
// Main control loop
// Reads joystick values and sends them to the robot every 20ms
int main(void) {
  setup();  // Init everything

  uint32_t lastSend = 0;   // Last time a packet was sent

  while (1) {
    // Check if 20ms has passed since last send
    // This limits packet rate to 50Hz (fast, but not overwhelming)
    if (milliseconds_now() - lastSend >= 20) {
      lastSend = milliseconds_now();
      
      sendSwitchOperation();
      requestBATTERYdata();
      requestLIGHTdata();
      requestRANGEdata();
      updateLCD();
      
      sendMotorControl();
      sendServoControl();


      // Read left joystick (X = turn, Y = forward/backward)
      // left_x_val = adc_read(PIN_JOYSTICKRIGHT_X);
      // left_y_val = adc_read(PIN_JOYSTICKRIGHT_Y); 
      // left_y_val = adc_read(PIN_JOYSTICKLEFT_Y); 
 
      // sprintf(msg, "\nNEW\nJoy X: %d\nJoy Y: %d\n", left_x_val, left_y_val);
      // serial0_print_string(msg);
      
      // motor_data_conversion(motor_d);
      
      // Send joystick data as a 5-byte command packet:
      // serial2_write_bytes(5, JOYSTICK_MOTOR_READ, motor_d[0], motor_d[1], motor_d[2], motor_d[3]);

      
      // servo_read_joystick(servo_d);
      // serial2_write_bytes(3, JOYSTICK_SERVO_READ, servo_d[0], servo_d[1]);

    }
  }
  return 0;
}