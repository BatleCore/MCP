//BATTERY VOLTAGE MONITORING

#include "battery.h"

// BATTERY SENSOR PORTS & PINS
#define PIN_BATTERY_SENSE PF0 // ADC pin

#define PORT_BATTERY PORTA  
#define DDR_BATTERY_LED DDRA
#define PIN_BATTERY_LED PA4   // Indicator LED digital output

#define BATTERY_THRESH 950    // seems to trip at 7.4v supply..?
uint16_t bat_val;             // stores battery voltage reading

void battery_init() {
  adc_init();                               // initialise ADC
  _delay_ms(20);                            // delay for safety

  DDR_BATTERY_LED |= (1<<PIN_BATTERY_LED);  // Configure digital output for LED control
  PORT_BATTERY &= ~(1<<PIN_BATTERY_LED);    // Initalise as off
}

void monitorBattery() {                      

  bat_val = adc_read(PIN_BATTERY_SENSE);    // get adc reading

  if (bat_val < BATTERY_THRESH) {
    PORT_BATTERY &= ~(1<<PIN_BATTERY_LED); // Voltage < 7V, flash LED 5 times a second
    _delay_ms(200);
    PORT_BATTERY |= (1<<PIN_BATTERY_LED);   
  } else {
    PORT_BATTERY &= ~(1<<PIN_BATTERY_LED);  // Voltage > 7V, turn LED off
  }
}

void testBattery() {
  // Monitor the battery
  monitorBattery();

  // Initialise serial
  serial0_init();
  char msg[20];
  
  // Send to PC serial for debugging
  sprintf(msg, "%d\n", bat_val);            
  serial0_print_string(msg);
}
