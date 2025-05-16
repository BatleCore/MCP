#include "light_sensor.h"
#include <math.h>

// these are imported with PIN_ASSIGNMENT.h
// #define PIN_LDR_LEFT PA4
// #define PIN_LDR_RIGHT PA4

int16_t last_signal=0;
volatile bool new_sample_ready = false;
volatile uint16_t leftLDR = 0;
volatile uint16_t rightLDR = 0;

char debug_msg[64];

void LDR_init() {
    TCCR3A = 0;
    TCCR3B = (1<<WGM32); // CTC Mode
    TCCR3B |= (1<<CS31)|(1<<CS30); // 64 Prescaler
    OCR3B = 2499; // 100hz ISR
    TIMSK3 |= (1<< OCIE3A); //enable the ISR
    sei(); // enable global ISRs
}
// call on interrupt
uint16_t getFrequency(uint16_t adc, uint8_t channel) {
    // Static variables persist across function calls
    static uint16_t baseline_left = 0;
    static uint16_t baseline_right = 0;
    uint16_t* baseline = (channel == 0) ? &baseline_left : &baseline_right; // Moving average baseline for ambient light
    
    static int16_t last_signal_left = 0;
    static int16_t last_signal_right = 0;
    int16_t* last_signal = (channel == 0) ? &last_signal_left : &last_signal_right; // Previous signal value for edge detection
    
    uint16_t freq = 0;              // Output frequency in hundredths of Hz (e.g. 1342 = 13.42 Hz)
    // Update baseline using exponential moving average to filter slow ambient changes
    *baseline = (*baseline * 9 + adc) / 10;
    // Signal = deviation from baseline (high-pass effect)
    int16_t signal = adc - *baseline;

    sprintf(debug_msg, "[CH %d] ADC: %u  Baseline: %u\n", channel, adc, *baseline);
    serial0_print_string(debug_msg);
    
    sprintf(debug_msg, "[CH %d] Signal: %d  Last: %d\n", channel, signal, *last_signal);
    serial0_print_string(debug_msg);

    // Detect a rising zero-crossing (negative → positive) that exceeds threshold
    if (last_signal < 0 && signal >= 0 && abs(signal) > SIGNAL_THRESHOLD) {
        freq = (uint32_t)100000 / SAMPLE_PERIOD;

        sprintf(debug_msg, "[CH %d] Zero-crossing → Freq: %u\n", channel, freq);
        serial0_print_string(debug_msg);
        }
    // Store current signal for next zero-crossing detection
    *last_signal = signal;
    // Return the frequency (0 if no new edge this sample)
    return freq;
}

ISR(TIMER3_COMPB_vect) {
    leftLDR = adc_read(PIN_LDR_LEFT);
    rightLDR = adc_read(PIN_LDR_RIGHT);
    new_sample_ready = true;
    sprintf(debug_msg, "[ISR] LDR_L: %u  LDR_R: %u\n", leftLDR, rightLDR);
    serial0_print_string(debug_msg);
}

void seekBeacon(uint16_t leftLDR, uint16_t rightLDR) {
    
    sprintf(debug_msg, "[seekBeacon] L: %u  R: %u\n", leftLDR, rightLDR);
    serial0_print_string(debug_msg);

    //uint8_t motor_data[4];
    //int bug[5];

    // Calculate turn direction
    uint16_t total_magnitude = leftLDR + rightLDR;
    uint16_t turn = (total_magnitude == 0) ? 512 : 1024 - (1024 * leftLDR / total_magnitude);
    

    
    // calculate spee
    uint16_t speed;
    uint16_t avg_magnitude = total_magnitude / 2;

    sprintf(debug_msg, "[seekBeacon] Total: %u  Avg: %u\n", total_magnitude, avg_magnitude);
    serial0_print_string(debug_msg);

    if (avg_magnitude >= PROXIMITY_THRESHOLD) {
        speed = 512;  // Stop or idle speed
        // Display "found" on LCD
    } else {
        // Scale speed linearly with distance
        // Far = fast, Near = slow
        speed = 1023 - avg_magnitude;
    }

    sprintf(debug_msg, "[seekBeacon] Turn: %u  Speed: %u\n", turn, speed);
    serial0_print_string(debug_msg);

    // Execute motor instruction
    //motor_data_conversion(speed, turn, motor_data, bug);
    //differential_PWM_v3(motor_data);
}

void LDR_test() {
    serial0_init();     // Needed to print to PC terminal
    LDR_init();
    adc_init();
    _delay_ms(20);
    uint16_t L_val = 0;
    uint16_t R_val = 0;
    char msg[32];

    while (1) {
        L_val = adc_read(PIN_LDR_LEFT);
        R_val = adc_read(PIN_LDR_RIGHT);
        sprintf(msg, "\n\nL: %d\nR: %d", L_val, R_val);
        serial0_print_string(msg);
        _delay_ms(250);
    }

}

void LDR_test2() {
    leftLDR = adc_read(PIN_LDR_LEFT);
    rightLDR = adc_read(PIN_LDR_RIGHT);
    sprintf(debug_msg, "[test2] LDR_L: %u  LDR_R: %u\n", leftLDR, rightLDR);
    serial0_print_string(debug_msg);
}