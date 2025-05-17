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

    if ((freqLeft >= 1900 && freqLeft <= 2100) ||
      (freqRight >= 1900 && freqRight <= 2100)) {
      seekBeacon(leftLDR, rightLDR);
    } else {
      speed = 512;
      turn = 512;
    }

      if (isr_counter >= 50) {
        isr_counter = 0;
        sprintf(msg, "\nLfreq: %d   Lval: %d", freqLeft, leftLDR);
        serial0_print_string(msg);
        sprintf(msg, "\nRfreq: %d   Rval: %d", freqRight, rightLDR);
        serial0_print_string(msg);

        sprintf(msg, "\nBeacon Turn: %u  Speed: %u\n", turn, speed);
        serial0_print_string(msg);

        _delay_ms(250);
      }

    

    
  }

  return 0;
}
