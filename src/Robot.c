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

  char msg[30]; // for debugging
  
  while (1) {

    // switch:case for AUTO and MANUAL modes
    switch (current_mode)
    {
    case AUTO_MODE:
      auto_init(); // eg. open servo
      // serial0_print_string("in automode");
      while (1) {
        // read auto serial
        serial_forAuto();
        if ( current_mode != AUTO_MODE ) {
          break;
        } else {
          auto_loop();
          _delay_ms(50); // test line, may be cycling too fast
          // do auto mode
        }
      }
      break;
    
    case MANUAL_MODE:
      serial0_print_string("in manual mode");
      while (1) {
        serial_forManual();
        // serial0_print_string("in manual loop/n");
        if ( current_mode != MANUAL_MODE ) {
          break;
        } else {
          // appears to be a delay in this cycle. motor controls have latency
          // do manual mode ( not much else to do right now... )
        }
      }      
      break;

    default:
      break;
    }
  }
  return 0;
}

void serial_forAuto() {
  if (serial2_available()) { // used when controlling via serial
    serial2_get_data(dataRX, PACKETSIZE);

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
      
      case MOTOR_CONTROL: {
        // not controlled in auto mode
        // motor_fromSerial(dataRX);
        break;
      }

      case SERVO_CONTROL: {
        // not controlled in auto mode
        // servo_set_velocity(dataRX);
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

void serial_forManual() {
  if (serial2_available()) { // used when controlling via serial
    serial2_get_data(dataRX, PACKETSIZE);

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
      
      case MOTOR_CONTROL: {
        motor_fromSerial(dataRX); // operates motors directly
        break;
      }

      case SERVO_CONTROL: {
        // serial0_print_string("servo sig recieved");
        servo_fromSerial(dataRX); // operates servo directly
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