// Lab 8 Robot Micro
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

// BATTERY SENSOR
#define PIN_BATTERY_SENSE PF0
#define PIN_BATTERY_LED PA4
#define DDR_BATTERY_LED DDRA
#define PORT_BATTERY PORTA
#define PORT_ADC PORTF
#define DDR_ADC DDRF
#define BATTERY_THRESH 300

// MOTOR PINS
#define PIN_ML_F PA0 // D22 → Left motor forward
#define PIN_ML_R PA1 // D23 → Left motor reverse
#define PIN_MR_F PA2 // D24 → Right motor forward
#define PIN_MR_R PA3 // D25 → Right motor reverse

// NEW MOTOR PINS
#define PIN_ML PA0 // D22 → Left motor
#define PIN_MR PA1 // D23 → Right motor

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


/********************
Robot Initialization
- Serial for debug (Serial0)
- Serial2 for XBee communication
- ADC for joystick & LDRs
- Timer for PWM
********************/
void setup() {
  cli();           // Disable interrupts during setup
  DDR_BATTERY_LED |= (1<<PIN_BATTERY_LED); // 
  // DDR_ADC &= ~(1<<PIN_BATTERY_SENSE); // battery pin ADC

  PORT_BATTERY &= ~(1<<PIN_BATTERY_LED);

  serial0_init();      // Debug serial
  serial2_init();      // Xbee serial

  motor_init();

  milliseconds_init();   // Millisecond timing
  // timerPWM_init();     // Timer1 PWM setup
  _delay_ms(20);
  adc_init();        // Analog input (joystick + sensors)
  _delay_ms(20);
  differential_PWM_init();
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
  uint8_t motor_d[4];
  motor_d[0] = 0;
  motor_d[1] = 0;
  motor_d[2] = 0;
  motor_d[3] = 0;
  int adc_bat_val = 0;

  char msg[30];
  uint32_t lastSend = 0;   // Last time a packet was sent
 
  while (1) {
    // Battery Reading
    adc_bat_val = adc_read(PIN_BATTERY_SENSE);
    if (adc_bat_val < BATTERY_THRESH) {
      PORT_BATTERY |= (1<<PIN_BATTERY_LED);
    } else {
      PORT_BATTERY &= ~(1<<PIN_BATTERY_LED);
    }

    // Receive Serial Coms
    if (serial2_available()) { // used when controlling via serial
      serial2_get_data(dataRX, PACKETSIZE);
 
      switch (dataRX[0]) {
        case LDR_REQUEST: {
          // Read and respond with LDR values
          // not currently used
          // not currently initialized
          int left_LDR_read = adc_read(LEFT_LDR_PIN);
          int right_LDR_read = adc_read(RIGHT_LDR_PIN);
          uint8_t left_light = LDRmap(left_LDR_read);
          uint8_t right_light = LDRmap(right_LDR_read);
          serial2_write_bytes(2, left_light, right_light);
          break;
        }
 
        case JOYSTICK_READ: {
          // Use joystick X/Y to update motor control
          motor_d[0] = dataRX[1];
          motor_d[1] = dataRX[2];
          motor_d[2] = dataRX[3];
          motor_d[3] = dataRX[4];
          differential_PWM_v3(motor_d);
          break;
        }
 
        default: {
          // Unrecognized command
          serial2_write_bytes(1, REQUEST_ERROR);
          break;
        }
      }
    }

    // Serial debug at interval
    if (milliseconds_now() - lastSend >= 500) {
      lastSend = milliseconds_now();
      // sprintf(msg, "\nL: %d : %d\n", motor_d[1], motor_d[0]);
      // serial0_print_string(msg);
      // sprintf(msg, "R: %d : %d\n", motor_d[3], motor_d[2]);
      // serial0_print_string(msg);
      sprintf(msg, "Battery reading:%d\n", adc_bat_val);
      serial0_print_string(msg);
    }

  }
  return 0;
}
