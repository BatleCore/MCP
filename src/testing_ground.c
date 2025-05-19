#include "auto_mode.h"
#include "Robot.h"

// NUNCHUK CONTROL
#define NUNCHUK_X 6
#define NUNCHUK_Y 7
#define NUNCHUK_Z PD0
#define NUNCHUK_C PD1

#define MODE 1 // 0: manual, 1: auto

void setup() {
  cli();           // Disable interrupts during setup
  serial0_init();      // Debug serial
  serial2_init();      // Xbee serial
  
  battery_init();
  motor_init();
  servo_init();

  milliseconds_init();   // Millisecond timing
  // timerPWM_init();     // Timer1 PWM setup
  _delay_ms(20);
  adc_init();        // Analog input (joystick + sensors)
  _delay_ms(20);
  sei();           // Enable interrupts
}

void cs_motor_conversion_NUNCHUK(uint8_t* results){
  // no input data - reads ADC pins directly
  // output matches input of "rs_motor_conversion"

  uint8_t travel_mag = 0; // absolute velocity  ( 0 -> 250 )
  uint8_t travel_dir = 0; // velocity direction ( forward, reverse )
  uint8_t turn_mag = 0; // absolute turning ( 0 -> 250 )
  uint8_t turn_dir = 0; // turning direction

  uint16_t speed = adc_read(NUNCHUK_Y);
  uint16_t turning = adc_read(NUNCHUK_X);


  /* TRIMMING FOR STICK-DRIFT */
  if (speed > centre_TOP)
  { // forward
    travel_mag = (speed - centre_TOP) * (250.0 / centre_BOT);
    travel_dir = (SPEED_INVERT);
  }
  else if (speed < centre_BOT)
  {
    travel_mag = (centre_BOT - speed) * (250.0 / centre_BOT);
    travel_dir = !(SPEED_INVERT);
  } else {
    travel_mag = 0;
    travel_dir = !(SPEED_INVERT);
  }

  if (turning > centre_TOP)
  { // RIGHT
    turn_mag = (turning - centre_TOP) * (250.0 / centre_BOT);
    turn_dir = (TURN_INVERT);
  }
  else if (turning < centre_BOT)
  { // LEFT
    turn_mag = (centre_BOT - turning) * (250.0 / centre_BOT);
    turn_dir = !(TURN_INVERT);
  } else { 
    turn_mag = 0;
    turn_dir = !(TURN_INVERT);
  }
  results[0] = travel_mag;
  results[1] = travel_dir;
  results[2] = turn_mag;
  results[3] = turn_dir;
}

int main(void){
  uint8_t motor_data[4];
  char msg[40];

  setup();

  switch (MODE)
  {
  case 0:
    // manual
    while(1) {
      cs_motor_conversion_NUNCHUK(motor_data);
      rs_motor_conversion(motor_data); // controls motors directly
      // sprintf(msg, "\n1: %d\n2: %d\n3: %d\n4: %d\n", motor_data[0], motor_data[1], motor_data[2], motor_data[3]);
      // serial0_print_string(msg);

      _delay_ms(20);
    }
    break;
  
  case 1:
    // auto
    while(1) {
      mode_2();
      _delay_ms(50);
    }
    break;
  
  default:
    break;
  }
}

