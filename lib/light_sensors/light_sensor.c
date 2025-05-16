#include "light_sensor.h"

// these are imported with PIN_ASSIGNMENT.h
// #define PIN_LDR_LEFT PA4
// #define PIN_LDR_RIGHT PA4

#define SAMPLE_INTERVAL_MS 5  // 200 Hz
#define THRESHOLD 20          // depends on ADC scale

int16_t last_signal = 0;
uint32_t last_edge_time = 0;

void LDR_init() {
    
}

void readLDR() {

}

uint16_t on_adc_sample(uint16_t adc) {
    // Static variables persist across function calls
    static uint16_t baseline = 0;         // Moving average baseline for ambient light
    static int16_t last_signal = 0;       // Previous signal value for edge detection
    static uint32_t last_edge_time = 0;   // Time of last rising edge (ms)
    static uint32_t time_ms = 0;          // Simulated time counter (increments per sample)

    uint16_t freq_fixed = 0;              // Output frequency in hundredths of Hz (e.g. 1342 = 13.42 Hz)

    // Update baseline using exponential moving average to filter slow ambient changes
    baseline = (baseline * 9 + adc) / 10;

    // Signal = deviation from baseline (high-pass effect)
    int16_t signal = adc - baseline;

    // Detect a rising zero-crossing (negative → positive) that exceeds threshold
    if (last_signal < 0 && signal >= 0 && abs(signal) > THRESHOLD) {
        uint32_t now = time_ms;
        uint32_t period_ms = now - last_edge_time;

        // Compute frequency in hundredths of Hz: freq = 1000.00 / period_ms
        if (period_ms > 0) {
            freq_fixed = (uint32_t)100000 / period_ms;
        }

        last_edge_time = now;
    }

    // Store current signal for next zero-crossing detection
    last_signal = signal;

    // Simulate a real-time clock tick (in practice, this should come from a timer)
    time_ms += SAMPLE_INTERVAL_MS;

    // Return the frequency (0 if no new edge this sample)
    return freq_fixed;
}

void LDR_test() {
    serial0_init();
    LDR_init();
    adc_init();
    _delay_ms(20);
    uint16_t L_val = 0;
    uint16_t R_val = 0;
    char msg[20];

    while (1) {
        L_val = adc_read(PIN_LDR_LEFT);
        R_val = adc_read(PIN_LDR_RIGHT);
        sprintf(msg, "\n\nL: %d\nR: %d", L_val, R_val);
        serial0_print_string(msg);
        _delay_ms(250);
    }


}