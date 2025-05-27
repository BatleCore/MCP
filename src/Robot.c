// Lab 8 Robot Micro
#include "Robot.h"

/********************
Robot Initialization
- Serial for debug (Serial0)
- Serial2 for XBee communication
- ADC for joystick & LDRs
- Timer for PWM
********************/
void robot_setup() {
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

  current_mode = MANUAL_MODE; // start in manual

  sei();           // Enable interrupts
}
 
/********************
Main Program Loop
********************/
int main(void) {

  robot_setup();
  //char msg[40]; // for debugging
  
  while (1) {

    monitorBattery(); // Monitor the battery voltage - common to either mode - mat

    // switch:case for AUTO and MANUAL modes
    switch (current_mode) {

      case AUTO_MODE: {

        auto_init(); // eg. open servo
        // serial0_print_string("in automode");
        
        while(1) {
          if (current_mode == MANUAL_MODE) {
            break; // exit while loop if mode switches
          }
          
          serial_forAuto(); // read auto serial
          auto_loop();
          _delay_ms(50); // test line, may be cycling too fast

        }

        break;
      }
    
      case MANUAL_MODE: {

        // manual init (if necassary)
        serial0_print_string("in manual mode");

        while (1) {
          if ( current_mode != MANUAL_MODE ) {
            break; // exit loop if mode switches
          }
          manual_loop(); // Coreys comment: appears to be a delay in this cycle. motor controls have latency
        }     

        break;
      }

      default: {
        // do nothing
        break;
      }

    }
  }
  return 0;
}

void serial_forAuto() {

  uint8_t dataRX[6];

  if (serial2_available()) { // used when controlling via serial
    serial2_get_data(dataRX, 6);

    switch (dataRX[0]) {

      case LIGHT_REQUEST: {
        sendLIGHTdata();
        break;
      }

      case RANGE_REQUEST: {
        sendRANGEdata();
        break;
      }

      case BATTERY_REQUEST: {
        sendBATTERYdata();
        break;
      }

      case MODE_SWITCH: {
        current_mode = dataRX[1]; // either AUTO_MODE or MANUAL_MODE
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