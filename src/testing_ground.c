//main
#include "light_sensor.h"
#include "motor_control.h"
#include "communication.h"
#include "controller_display.h"
#include "servo_control.h"
#include "manual_mode.h"

int main(void) {
    cli();  // Disable global interrupts

    // Initialize subsystems
    motor_init();
    serial0_init();
    milliseconds_init();
    LDR_init();
    adc_init();

    _delay_ms(20);  // Allow time for peripherals to stabilize
    

    sei();  // Enable global interrupts (important!)

    char msg[32];

    while (1) {
        // Beacon detected (frequency match)
        if ((freqLeft >= 1900 && freqLeft <= 2100) ||
            (freqRight >= 1900 && freqRight <= 2100)) {
            seekBeacon(leftLDR, rightLDR, signalLeft, signalRight);  // Smart movement toward beacon
        } else {
            uint8_t motor_data[4] = {0};
            rs_motor_conversion(motor_data);
        }

        // Periodic debug print every 50 ISR ticks
        if (isr_counter >= 50) {
            isr_counter = 0;

            sprintf(msg, "\nLfreq: %d   Lval: %d", freqLeft, leftLDR);
            serial0_print_string(msg);

            sprintf(msg, "\nRfreq: %d   Rval: %d", freqRight, rightLDR);
            serial0_print_string(msg);

            sprintf(msg, "\nLsig %d Rsig: %d", signalLeft, signalRight);
            serial0_print_string(msg);

            sprintf(msg, "\nLbase %d Rbase: %d", baselineLeft, baselineRight);
            serial0_print_string(msg);

            sprintf(msg, "\nBeacon Turn: %u  Speed: %u\n", turn, speed);
            serial0_print_string(msg);

            _delay_ms(250);  // Slow down debug output
        }
    }

    return 0;
}
  