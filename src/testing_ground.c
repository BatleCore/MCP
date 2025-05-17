//main
#include "light_sensor.h"

int main(void) {
  cli();
  motor_init();
  serial0_init();
  milliseconds_init();
  LDR_init();
  adc_init();
  _delay_ms(20);
  char msg[32];


  while (1) {
    if (new_LDR_readings) {
      new_LDR_readings = false;

      signalLeft = getSignal(leftLDR, 0);
      signalRight = getSignal(rightLDR, 1);

      freqLeft = getFrequency(signalLeft, 0);
      freqRight = getFrequency(signalRight, 1);

      if (isr_counter >= 50) {
        isr_counter = 0;
        sprintf(msg, "\n\nLval: %d\nRval: %d", leftLDR, rightLDR);
        serial0_print_string(msg);

        sprintf(msg, "\n\nLsignal: %d\nRsignal: %d", signalLeft, signalRight);
        serial0_print_string(msg);

        sprintf(msg, "\n\nLfreq: %d\nRfreq: %d", freqLeft, freqRight);
        serial0_print_string(msg);
        _delay_ms(250);
      }
    }
    

    
  }

  return 0;
}
