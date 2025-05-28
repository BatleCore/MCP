// Lab 8 Robot Micro
#include "manual_mode.h"



void manual_loop() {
 
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
      
      case MOTOR_CONTROL: {
        for (int i = 0; i < 4; i++) {
          motor_data[i] = dataRX[i+1];
        }
        // motor_fromSerial(dataRX); // operates motors directly
        break;
      }

      case SERVO_CONTROL: {
        servo_fromSerial(dataRX);

        //servo_fromSerial(dataRX); // operates servo directly
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

  // outside of switch case to eliminate serial delay lag
  rs_motor_conversion();

}
