//BATTERY VOLTAGE MONITORING

#include "battery.h"

void battery_init() {
  DDR_BATTERY_LED |= (1<<PIN_BATTERY_LED);  // Configure digital output for LED control
  PORT_BATTERY &= ~(1<<PIN_BATTERY_LED);    // Initalise as off
  OCR3A = 2499; // 100hz ISR
  TIMSK3 |= (1<< OCIE3A); //enable the ISR
  sei(); // enable global ISRs
}

uint8_t mapBatVoltage(uint16_t adc_value) {
    return (adc_value * 84) / 1023;
}

ISR(TIMER3_COMPA_vect) {
  battery_check_counter ++; // counter to check battery once per second
  battery_flash_counter++;

  if (battery_check_counter >= CHECK_TIME) {
    battery_check_counter = 0;
    bat_val = adc_read(PIN_BATTERY_SENSE);
    bat_low = (bat_val <= BATTERY_THRESH);
  }

  if (bat_low && battery_flash_counter >= FLASH_TIME) {
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
