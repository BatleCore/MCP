
// CONTROLLER
// Lab 8 Controller Micro
#include "Controller.h"
// Joystick Analog Pins (forgot how this works on controller)
#define PIN_JOYSTICKLEFT_X 0  // ADC0
#define PIN_JOYSTICKLEFT_Y 1  // ADC1
#define PIN_JOYSTICKRIGHT_X 2 // ADC2 (unused)
#define PIN_JOYSTICKRIGHT_Y 3 // ADC3 (unused)

// Communication Codes (matches the robot)
#define LDR_REQUEST  0xA0  // Request light sensor data from robot
#define JOYSTICK_READ  0xA1  // Send joystick X/Y to robot for motor control
#define REQUEST_ERROR  0xEE  // Error / fallback code (not used here)

// Convert 10-bit ADC (0–1023) to 8-bit value (0–255)
uint8_t mapADC(uint16_t adc_val) {
  return adc_val >> 2; // divide by 4
}

// System setup
// - Initializes communication (serial2 for Xbee)
// - Initializes ADC and millisecond timing
void setup() {
  cli();           // Disable interrupts while setting up
  serial2_init();      // XBee serial communication
  milliseconds_init();   // For timing control
  _delay_ms(20);
  adc_init();        // Enable analog input (for joysticks)
  _delay_ms(20);
  sei();           // Enable global interrupts
}
 
// Main control loop
// Reads joystick values and sends them to the robot every 20ms
int main(void) {
  setup();  // Init everything
 
  uint16_t left_x_val;   // Raw ADC reading for joystick X
  uint16_t left_y_val;   // Raw ADC reading for joystick Y
  uint32_t lastSend = 0;   // Last time a packet was sent
 
  while (1) {
    // Check if 20ms has passed since last send
    // This limits packet rate to 50Hz (fast, but not overwhelming)
    if (milliseconds_now() - lastSend >= 20) {
      lastSend = milliseconds_now();
 
      // Read left joystick (X = turn, Y = forward/backward)
      left_x_val = adc_read(PIN_JOYSTICKLEFT_X);
      left_y_val = adc_read(PIN_JOYSTICKLEFT_Y);
 
      // Send joystick data as a 3-byte command packet:
      // [Command Code, X value, Y value]
      serial2_write_bytes(3, JOYSTICK_READ, mapADC(left_x_val), mapADC(left_y_val));
    }
  }
 
  return 0;
}

void print_to_lcd(char * msg1, int16_t val1, int16_t val2)
{
  char LCD_msg[17] = {0};
  lcd_clrscr();
  lcd_goto(0);
  snprintf(LCD_msg, sizeof(LCD_msg), "X: %d, Y: %d", val1, val2);
  lcd_puts(LCD_msg);
  lcd_goto(0x40);
  snprintf(LCD_msg, sizeof(LCD_msg), "%s", msg1);
  lcd_puts(LCD_msg);
}
