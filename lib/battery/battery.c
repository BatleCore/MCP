//BATTERY VOLTAGE MONITORING
#include "battery.h"

void battery_init() {
  DDR_BATTERY_LED |= (1<<PIN_BATTERY_LED);  // Configure digital output for LED control
  PORT_BATTERY &= ~(1<<PIN_BATTERY_LED);    // Initalise as off
}

void monitorBattery() {

  static uint32_t lastTime[2];
  uint32_t currTime = milliseconds_now();

    if (currTime - lastTime[0] >= CHECK_TIME) {
    bat_val = adc_read(PIN_BATTERY_SENSE);
    bat_low = (bat_val <= BATTERY_THRESH);
    lastTime[0] = currTime;
  }

  if (bat_low && currTime - lastTime[1] >= FLASH_TIME) {
    PORT_BATTERY ^= (1<<PIN_BATTERY_LED); // switch on/off
    lastTime[1] = currTime;
  }
}

// Returns ADC converted to voltage (0-8.4V)
uint8_t getVoltage() {
  return (bat_val * 84UL) / 1023;
}

// Debugging Function - Prints raw ADC values
void testBattery() {
  bat_val = adc_read(PIN_BATTERY_SENSE);
  char msg[20];
  sprintf(msg, "%d\n", bat_val);            
  serial0_print_string(msg);
}
