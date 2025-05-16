//main
#include "light_sensor.h"


int main(void) {
    serial0_init();
    lcd_init();
    adc_init();
    _delay_ms(20);

    while (1) {
      LDR_test2();
      _delay_ms(250);
    }
    return 0;
}

/*
int main(void) {
  cli();
  motor_init();
  serial0_init();
  milliseconds_init();
  LDR_init();
  adc_init();
  _delay_ms(20);

  while (1) {
    if (new_sample_ready) {
      new_sample_ready = false;

      uint16_t freqLeft = getFrequency(leftLDR, 0);
      uint16_t freqRight = getFrequency(rightLDR, 1);

      if (abs(freqLeft - FREQ_TARGET) < FREQ_TOL &&
          abs(freqRight - FREQ_TARGET) < FREQ_TOL) {
          seekBeacon(leftLDR, rightLDR);
      }
    }
  }

    return 0;
} */
