// Lab 8 Robot Micro
#include "Robot.h"

/********************
Robot Initialization
- Serial for debug (Serial0)
- Serial2 for XBee communication
- ADC for joystick & LDRs
- Timer for PWM
********************/
void setup() {
  cli();           // Disable interrupts during setup
  
  // communication channels
  serial0_init();      // Debug serial
  serial2_init();      // Xbee serial

  // features
  battery_init(); 
  comms_init();
  display_init();
  LDR_init();
  motor_init();
  servo_init();

  milliseconds_init();   // Millisecond timing
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
 
  uint8_t dataRX[PACKETSIZE]; // move to Robot.h?

  uint8_t servo_d[2]; // to be made obsolete in servo.h
  servo_d[0] = 0;
  servo_d[1] = 0;

  char msg[30]; // for debugging
  
  while (1) {

    // switch:case for AUTO and MANUAL modes
    switch (current_mode)
    {
    case AUTO_MODE:
      while (1) {
        // read auto serial
        if ( current_mode != AUTO_MODE ) {
          break;
        } else {
          // do auto mode
        }
      }
      break;
    
    case MANUAL_MODE:
      while (1) {
        // read auto serial
        if ( current_mode != MANUAL_MODE ) {
          break;
        } else {
          // do manual mode
        }
      }      
      break;

    default:
      break;
    }

    // Receive Serial Coms
    if (serial2_available()) { // used when controlling via serial
      serial2_get_data(dataRX, PACKETSIZE);
 
      switch (dataRX[0]) {
 
        case JOYSTICK_MOTOR_READ: {
          // Use joystick X/Y to update motor control
          motor_fromSerial(dataRX);
          break;
        }

        case JOYSTICK_SERVO_READ: { // this needs a deep dive to clean
          servo_d[0] = dataRX[1];
          servo_d[1] = dataRX[2];
          servo_set_velocity(servo_d);
          sprintf(msg, "\nvel: %d, dir: %d", servo_d[0], servo_d[1]);
          serial0_print_string(msg);
          break;
        }

        default: {
          // Unrecognized command
          serial2_write_bytes(1, REQUEST_ERROR);
          break;
        }
      }
    }
  }
  return 0;
}
