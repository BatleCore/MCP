// ADC LED BATERRY LEVEL INDICATOR

#include "test_battery.h"


// BATTERY SENSOR
#define PIN_BATTERY_SENSE PF0
#define PORT_ADC PORTF
#define DDR_ADC DDRF

#define PIN_BATTERY_LED PA4
#define PORT_BATTERY PORTA
#define DDR_BATTERY_LED DDRA

#define BATTERY_THRESH 950 // seems to trip at 7.4v supply..?

void send_ADC_to_serial(){
  adc_init();        // Analog input (joystick + sensors)
  _delay_ms(20);
  serial0_init();

  // output
  DDR_BATTERY_LED |= (1<<PIN_BATTERY_LED); // output HIGH
  PORT_BATTERY &= ~(1<<PIN_BATTERY_LED); // default LOW

  uint16_t bat_val;
  char msg[20];

  while(1) {
    bat_val = adc_read(PIN_BATTERY_SENSE);

    if (bat_val < BATTERY_THRESH) {
      PORT_BATTERY |= (1<<PIN_BATTERY_LED);
    } else {
      PORT_BATTERY &= ~(1<<PIN_BATTERY_LED);
    }

    sprintf(msg, "%d\n", bat_val);
    serial0_print_string(msg);
    _delay_ms(200);
  }
}