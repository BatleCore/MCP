//BATTERY VOLTAGE MONITORING

#include "battery.h"

// BATTERY SENSOR PORTS & PINS
#define PIN_BATTERY_SENSE PF0 // ADC pin

#define PORT_BATTERY PORTA  
#define DDR_BATTERY_LED DDRA
#define PIN_BATTERY_LED PA4   // Indicator LED digital output

#define BATTERY_THRESH 950    // seems to trip at 7.4v supply..?
uint16_t bat_val;             // stores battery voltage reading
volatile uint8_t battery_check_counter;
volatile uint8_t battery_flash_counter;
volatile bool bat_low;

void battery_init() {
  DDR_BATTERY_LED |= (1<<PIN_BATTERY_LED);  // Configure digital output for LED control
  PORT_BATTERY &= ~(1<<PIN_BATTERY_LED);    // Initalise as off
  OCR3A = 2499; // 100hz ISR
  TIMSK3 |= (1<< OCIE3A); //enable the ISR
  sei(); // enable global ISRs
}

ISR(TIMER3_COMPA_vect) {
  battery_check_counter ++;
  battery_flash_counter++;

  if (battery_check_counter >= 100) {
    battery_check_counter = 0;
    bat_val = adc_read(PIN_BATTERY_SENSE);
    bat_low = (bat_val <= BATTERY_THRESH);
  }

  if (bat_low && battery_flash_counter>=25) {
    battery_flash_counter = 0;
    PORT_BATTERY ^= (1<<PIN_BATTERY_LED); // switch on/off
  }
}

void testBattery() {
  // Monitor the battery
  bat_val = adc_read(PIN_BATTERY_SENSE);
  // Send to PC serial for debugging
  char msg[20];
  sprintf(msg, "%d\n", bat_val);            
  serial0_print_string(msg);
}
