// Lab 8 Robot Micro
#include "Robot.h"
 
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
  uint8_t servo_d[2];

  char msg[30];
  uint32_t lastSend = 0;   // Last time a packet was sent
 
  while (1) {

    // Receive Serial Coms
    if (serial2_available()) { // used when controlling via serial
      serial2_get_data(dataRX, PACKETSIZE);
 
      switch (dataRX[0]) {
        case LDR_REQUEST: {
          // Read and respond with LDR values
          // not currently used
          // not currently initialized
          int left_LDR_read = adc_read(PIN_LDR_LEFT);
          int right_LDR_read = adc_read(PIN_LDR_RIGHT);
          uint8_t left_light = LDRmap(left_LDR_read);
          uint8_t right_light = LDRmap(right_LDR_read);
          serial2_write_bytes(2, left_light, right_light);
          break;
        }
 
        case JOYSTICK_MOTOR_READ: {
          // Use joystick X/Y to update motor control
          motor_d[0] = dataRX[1];
          motor_d[1] = dataRX[2];
          motor_d[2] = dataRX[3];
          motor_d[3] = dataRX[4];
          differential_PWM_v3(motor_d);
          break;
        }

        case JOYSTICK_SERVO_READ: {
          servo_d[0] = dataRX[0];
          servo_d[1] = dataRX[1];
          servo_set_velocity(servo_d);
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
