#include "light_sensor.h"

volatile uint32_t isr_counter = 0;

volatile uint16_t leftLDR = 0;
volatile uint16_t rightLDR = 0;
volatile bool new_LDR_readings = false;

volatile int16_t signalLeft = 0;
volatile int16_t signalRight = 0;

volatile uint16_t freqLeft = 0;
volatile uint16_t freqRight = 0;

uint16_t turn = 512;
uint16_t speed = 512;

char debug_msg[40];

void LDR_init() {
    milliseconds_init();
    TCCR4A = 0;
    TCCR4B = (1<<WGM42)|(1<<CS41); // CTC Mode Prescale 8
    OCR4A = 9999; // 200hz ISR
    TIMSK4 |= (1<< OCIE4A); //enable the ISR
    sei(); // enable global ISRs
}

int16_t getSignal(uint16_t LDRval, uint8_t channel) {

    static uint16_t baseline[2] = {0};
    baseline[channel] = (baseline[channel] * 9 + LDRval) / 10; // Update baseline using exponential moving average
    return (int16_t)LDRval - (int16_t)baseline[channel]; // Return signed deviation from baseline
}

uint16_t getFrequency(int16_t signal, uint8_t channel) {

    static int16_t  last_signal[2]    = {0};
    static uint32_t last_time[2]      = {0};
    static uint16_t last_freq[2]      = {0};
    static bool     new_edge[2]       = {0};
    static uint16_t edge_counter[2]   = {0};

    uint32_t now = milliseconds_now();
    if (last_signal[channel] < 0 && signal >= 0 && signal < SIGNAL_THRESHOLD) {
        uint32_t dt = now - last_time[channel];
        last_time[channel] = now;
        if (dt > 47) {
            new_edge[channel] = true;
            edge_counter[channel] = 0;
            last_freq[channel] = (200000UL) / dt;
        }
    }
    edge_counter[channel]++;
    if (edge_counter[channel] > 400 && !new_edge[channel]) {
        last_freq[channel] = 0;
    }
    new_edge[channel] = false;
    last_signal[channel] = signal;

    return last_freq[channel];
}

ISR(TIMER4_COMPA_vect) {

    isr_counter++;

    leftLDR = adc_read(PIN_LDR_LEFT);
    rightLDR = adc_read(PIN_LDR_RIGHT);

    signalLeft = getSignal(leftLDR, 0);
    signalRight = getSignal(rightLDR, 1);

    freqLeft = getFrequency(signalLeft, 0);
    freqRight = getFrequency(signalRight, 1);
}

void seekBeacon(uint16_t leftLDR, uint16_t rightLDR) {
    uint8_t motor_data[4];
    int bug[5];

    // Calculate turn direction
    uint16_t total_magnitude = leftLDR + rightLDR;
    turn = (total_magnitude == 0) ? 512 : (1024 * leftLDR) / (total_magnitude);
    
    // calculate speed
    
    if (total_magnitude == 0) { // Prevent divide-by-zero just in case
        turn = 512;
    } else if (leftLDR > rightLDR) {
        // More light on the LEFT → turn RIGHT → turn > 512
        turn = 512 + (512UL * (leftLDR - rightLDR)) / total_magnitude;
    } else {
        // More light on the RIGHT → turn LEFT → turn < 512
        turn = 512 - (512UL * (rightLDR - leftLDR)) / total_magnitude;
    }

    // Execute motor instruction
    motor_data_conversion(speed, turn, motor_data, bug);
    differential_PWM_v3(motor_data);
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