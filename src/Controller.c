// CONTROLLER
#include "Controller.h"

uint8_t robot_mode = MANUAL_MODE;

// System setup
void controller_setup() {
  cli();                // Disable interrupts while setting up
  serial2_init();       // XBee serial communication
  serial0_init();       // Terminal Serial Monitor
  milliseconds_init();  // For timing control
  display_init();       // Initialise LCD updates
  motor_init_params();  // Initialise motors
  _delay_ms(20);
  adc_init();           // Enable analog inputs (for joysticks)
  _delay_ms(20);

  // ADC PINS SET TO INPUT
  DDRK &= ~(1<<PIN_JOY_L_X);
  DDRK &= ~(1<<PIN_JOY_L_Y);
  DDRF &= ~(1<<PIN_JOY_R_X);
  DDRF &= ~(1<<PIN_JOY_R_X);
  

  // ISR for Left Joystick Button (Switching Operation Modes)
  DDRD &= ~(1 << PIN_JOY_L_BUTTON);
  PORTD |= (1 << PIN_JOY_L_BUTTON);
  EIMSK |= (1 << INT1);             // Enable INT0
  EICRA = (EICRA & ~(1 << ISC10))|(1 << ISC11);   // ISC01=1, ISC00=0 (falling edge)
  sei();           // Enable global interrupts
}
 
// Main control loop
// Reads joystick values and sends them to the robot every 20ms
int main(void) {
  controller_setup();  // Init everything

  uint32_t last_LCD_update = 0;   // Last time LCD was updated
  uint32_t last_control_update = 0;   // Last time control was sent to robot

  while (1) {

    // Update robot with controls every 50ms when in manual mode (20x/sec)
    if ((robot_mode == MANUAL_MODE) && (milliseconds_now() - last_control_update >= CONTROL_RATE)) {
      sendMotorControl();
      sendServoControl();
      last_control_update = milliseconds_now();
    }

    // Update LCD contents every 250ms (4x/sec)
    if (milliseconds_now() - last_LCD_update >= LCD_RATE) {
      updateLCD();
      last_LCD_update = milliseconds_now();
    }
  }
  return 0;
}

// ISR for screen switch button (right joystick)
ISR(INT1_vect) {
    
    static uint32_t last_press = 0; // Software debouncing

    if (milliseconds_now() - last_press > 250) {

      last_press = milliseconds_now();  // Reset software debouncing

      if (robot_mode == MANUAL_MODE) {
        robot_mode = AUTO_MODE;  // Switch from manual to auto
      } else {
        robot_mode = MANUAL_MODE;  // Switch from auto to manual
      }
      sendSwitchOperation(robot_mode);
    }
}