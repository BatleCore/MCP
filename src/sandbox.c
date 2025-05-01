// Flash D22–D25 (PORTA bits 0–3) in sync every 500ms

#include <avr/io.h>
#include <util/delay.h>

// Define motor control pins (matches your project structure)
#define PORT_CONTROL PORTA
#define DDR_CONTROL DDRA

#define PIN_ML_F PA0 // D22
#define PIN_ML_R PA1 // D23
#define PIN_MR_F PA2 // D24
#define PIN_MR_R PA3 // D25

void setup() {
  // Set D22–D25 (PA0–PA3) as output
  DDR_CONTROL |= (1 << PIN_ML_F) | (1 << PIN_ML_R) | (1 << PIN_MR_F) | (1 << PIN_MR_R);
}

int main(void) {
  setup();

  while (1) {
    // Turn ON all four pins (set bits HIGH)
    PORT_CONTROL |= (1 << PIN_ML_F) | (1 << PIN_ML_R) | (1 << PIN_MR_F) | (1 << PIN_MR_R);
    _delay_ms(500);

    // Turn OFF all four pins (set bits LOW)
    PORT_CONTROL &= ~((1 << PIN_ML_F) | (1 << PIN_ML_R) | (1 << PIN_MR_F) | (1 << PIN_MR_R));
    _delay_ms(500);
  }

  return 0;
}
