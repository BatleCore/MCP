// Lab 8 Robot Micro DUPLICATE
#include "Robot.h"
#include "motor_control.h"

// Constants
#define PACKETSIZE 6  // Max serial packet size we expect
 
// H-Bridge & Motor Control
#define PORT_PWM PORTB
#define DDR_PWM DDRB
#define PIN_PWM_ML PB5 // D11 → OC1A (PWM output for left motor)
#define PIN_PWM_MR PB6 // D12 → OC1B (PWM output for right motor)
#define PORT_CONTROL PORTA
#define DDR_CONTROL DDRA

// MOTOR PINS
#define PIN_ML_F PA0 // D22 → Left motor forward
#define PIN_ML_R PA1 // D23 → Left motor reverse
#define PIN_MR_F PA2 // D24 → Right motor forward
#define PIN_MR_R PA3 // D25 → Right motor reverse

// NEW MOTOR PINS
#define PIN_ML PA0 // D23 → Left motor
#define PIN_MR PA1 // D25 → Right motor

// CLOCK COMPLARE FOR PWMs
#define DUTY_LEFT OCR1A
#define DUTY_RIGHT OCR1B
 
// Communication Command Codes
#define LDR_REQUEST 0xA0   // Controller is asking for light sensor data
#define JOYSTICK_READ 0xA1   // Controller is sending joystick values
#define REQUEST_ERROR 0xEE   // For handling invalid codes
 
// Light Sensor ADC Channels
#define LEFT_LDR_PIN 0
#define RIGHT_LDR_PIN 1
 
// POT ADC Channels
#define LEFT_POT_PIN 2
#define RIGHT_POT_PIN 3

/********************
Timer1 PWM Setup
Phase and Frequency Correct PWM Mode (mode 8)
Prescaler = 8, ICR1 = 2000 → 500Hz PWM frequency
Used for motor speed control
********************/
void timerPWM_init() {
  /*
  PWM MAX = 2000
  PWM freq = 500Hz
  PWM period = 2ms
  */
  TCCR1A = (1<<COM1A1)|(1<<COM1B1);   // Non-inverting PWM on OC1A and OC1B
  TCCR1B = (1 << WGM13) | (1 << CS11);  // Mode 8: PWM Phase & Freq Correct, Prescaler = 8
  ICR1 = 2000;              // Set TOP for 500Hz
  OCR1A = 0; // LEFT
  OCR1B = 0; // RIGHT
  DDR_PWM |= (1 << PIN_PWM_ML) | (1 << PIN_PWM_MR); // Set PWM pins as output
}
 
// Scale raw LDR reading (0–1023) to 0–255
uint8_t LDRmap(int reading) {
  return reading >> 2;
}
 
/********************
Joystick → Motor Mapping
- Converts joystick (X/Y) into motor PWM duty cycle and direction
- Handles turning and direction flipping logic
********************/
void differential_PWM(uint8_t x, uint8_t y) {
  /*
  this will be rebuilt to use motor control data:
    - left_duty [0 -> 255]
    - left_dir  [0, 1]
    - right_duty [0 -> 255]
    - right_dir  [0, 1]
  */
  uint8_t leftRatio = 100;
  uint8_t rightRatio = 100;
  uint8_t speedRatio = 0;
  uint16_t leftPWM = 0;
  uint16_t rightPWM = 0;
 
  // Determine left/right scaling based on X-axis
  if (x < 127) { // Joystick pushed left
    leftRatio = 100 * x / 127;
    rightRatio = 100;
  }
  else if (x > 128) { // Joystick pushed right
    leftRatio = 100;
    rightRatio = 100 * (255 - x) / 127;
  }
 
  // Calculate motor speeds based on Y-axis
  if (y > 128) { // Forward
    speedRatio = 100 * (y - 128) / 127;
    leftPWM = 2 * speedRatio * leftRatio;
    rightPWM = 2 * speedRatio * rightRatio;
 
    // Set direction: forward
    PORT_CONTROL |= (1 << PIN_ML_F);
    PORT_CONTROL &= ~(1 << PIN_ML_R);
    PORT_CONTROL |= (1 << PIN_MR_F);
    PORT_CONTROL &= ~(1 << PIN_MR_R);
  }
  else if (y < 127) { // Backward
    speedRatio = 100 * y / 127;
    leftPWM = 2 * speedRatio * rightRatio;
    rightPWM = 2 * speedRatio * leftRatio;
 
    // Set direction: reverse
    PORT_CONTROL |= (1 << PIN_ML_R);
    PORT_CONTROL &= ~(1 << PIN_ML_F);
    PORT_CONTROL |= (1 << PIN_MR_R);
    PORT_CONTROL &= ~(1 << PIN_MR_F);
  }
 
  // Write final PWM duty cycles to Timer1
  OCR1A = leftPWM;
  OCR1B = rightPWM;
}

void differential_PWM_v3(uint8_t* motor_data){
  /*
  [0] = left_duty
  [1] = left_dir
  [2] = right_duty
  [3] = right_dir

  dir:
    2 = forward
    1 = stationary
    0 = reverse
  */

  char msg[20];

  uint16_t left_duty = motor_data[0] * (2000/255.0);
  uint8_t left_dir = motor_data[1];
  uint16_t right_duty = motor_data[2] * (2000/255.0);
  uint8_t right_dir = motor_data[3];

  sprintf(msg, "\nL: %d\t%d\n", left_dir, left_duty);
  serial0_print_string(msg);
  sprintf(msg, "R: %d\t%d", right_dir, right_duty);
  serial0_print_string(msg);

  if( left_dir == 2) { // Forward
    OCR1A = left_duty;
    PORT_CONTROL &= ~(1 << PIN_ML); // LOW
  } else if ( left_dir == 0 ) { // Reverse
    OCR1A = 2000 - left_duty;
    PORT_CONTROL |= (1 << PIN_ML); // HIGH
  } else { // locked LOW
    OCR1A = 0;
    PORT_CONTROL &= ~(1 << PIN_ML); // LOW
  }

  if( right_dir == 2 ) { // Forward
    OCR1B = right_duty;
    PORT_CONTROL &= ~(1 << PIN_MR); // LOW
  } else if ( right_dir == 0 ) { // Reverse
    OCR1B = 2000 - right_duty;
    PORT_CONTROL |= (1 << PIN_MR); // LOW
  } else { // locked LOW
    OCR1B = 0;
    PORT_CONTROL &= ~(1 << PIN_MR); // LOW
  }
}

/********************
Robot Initialization
- Serial for debug (Serial0)
- Serial2 for XBee communication
- ADC for joystick & LDRs
- Timer for PWM
********************/
void setup() {
  cli();           // Disable interrupts during setup
  DDR_CONTROL |= (1<<PIN_ML_F)|(1<<PIN_ML_R)|(1<<PIN_MR_F)|(1<<PIN_MR_R); // only using 2 of these
  DDR_CONTROL |= (1<<PIN_ML)|(1<<PIN_ML); // to replace the above line
  serial0_init();      // Debug serial
  serial2_init();      // Xbee serial - Robot to Controller
  milliseconds_init();   // Millisecond timing
  timerPWM_init();     // Timer1 PWM setup
  _delay_ms(20);
  adc_init();        // Analog input (joystick + sensors)
  _delay_ms(20);
  sei();           // Enable interrupts
}
 
/********************
Main Program Loop
- Waits for incoming packet over Serial2
- Responds based on command type:
  → JOYSTICK_READ: control motors
  → LDR_REQUEST: return light sensor values
********************/
int main(void) {
  setup();
 
  uint8_t dataRX[PACKETSIZE];
  int x_val = 0;
  int y_val = 0;


  int joy_L;
  int joy_R;
  uint8_t motor_data[4]; // stores converted motor data. typically recieved from serial Controller
  int debug_data[5]; // debug data from "motor_data_conversion"
  uint32_t lastSend = 0;   // time record for loop


  while (1) {
    // testing stage - this will be false
    if (serial2_available()) { // used when controlling via serial
      serial2_get_data(dataRX, PACKETSIZE);
 
      switch (dataRX[0]) {
        case LDR_REQUEST: {
          // Read and respond with LDR values
          int left_LDR_read = adc_read(LEFT_LDR_PIN);
          int right_LDR_read = adc_read(RIGHT_LDR_PIN);
          uint8_t left_light = LDRmap(left_LDR_read);
          uint8_t right_light = LDRmap(right_LDR_read);
          serial2_write_bytes(2, left_light, right_light);
          break;
        }
 
        case JOYSTICK_READ: {
          // Use joystick X/Y to update motor control
          x_val = dataRX[1];
          y_val = dataRX[2];
          differential_PWM(x_val, y_val);
          break;
        }
 
        default: {
          // Unrecognized command
          serial2_write_bytes(1, REQUEST_ERROR);
          break;
        }
      }
    }
    
    // testing stage - motor control is local, not from controller
    if (milliseconds_now() - lastSend >= 20) {
      lastSend = milliseconds_now();

      joy_L = adc_read(LEFT_POT_PIN);
      joy_R = adc_read(RIGHT_POT_PIN);
      motor_data_conversion(joy_L, joy_R, motor_data, debug_data);
      differential_PWM_v3(motor_data);
    }
  }
 
  return 0;
}
