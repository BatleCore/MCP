#include "light_sensor.h"

volatile uint32_t isr_counter = 0;

volatile uint16_t leftLDR = 0;
volatile uint16_t rightLDR = 0;
volatile bool new_LDR_readings = false;

volatile int16_t signalLeft = 0;
volatile int16_t signalRight = 0;

volatile uint16_t freqLeft = 0;
volatile uint16_t freqRight = 0;

char debug_msg[40];

void LDR_init() {
    milliseconds_init();
    TCCR4A = 0;
    TCCR4B = (1<<WGM42)|(1<<CS41); // CTC Mode Prescale 8
    OCR4A = 9999; // 200hz ISR
    TIMSK4 |= (1<< OCIE4A); //enable the ISR
    sei(); // enable global ISRs
}

uint16_t getLDRval(int pin) {
    return adc_read(pin);
}

int16_t getSignal(uint16_t LDRval, uint8_t channel) {
    static uint16_t baseline_left = 0;
    static uint16_t baseline_right = 0;

    uint16_t* baseline = (channel == 0) ? &baseline_left : &baseline_right;

    // Update baseline using exponential moving average
    *baseline = (*baseline * 4 + LDRval) / 5;

    // Return signed deviation from baseline
    return (int16_t)LDRval - (int16_t)(*baseline);
}

uint16_t getFrequency(int16_t signal, uint8_t channel) {
    static int16_t last_signal_left = 0;
    static int16_t last_signal_right = 0;
    static uint32_t last_time_left = 0;
    static uint32_t last_time_right = 0;
    static uint16_t last_freq_left = 0;
    static uint16_t last_freq_right = 0;

    int16_t* last_signal = (channel == 0) ? &last_signal_left : &last_signal_right;
    uint32_t* last_time = (channel == 0) ? &last_time_left : &last_time_right;
    uint16_t* freq = (channel == 0) ? &last_freq_left : &last_freq_right;

    uint32_t now = milliseconds_now();

    if (*last_signal < 0 && signal >= 0) {
        uint32_t dt = now - *last_time;
        *last_time = now;
        if (dt > 0) {
            *freq = (200000UL) / dt; // frequency in hundredths of Hz
        }
    }

    *last_signal = signal;

    return *freq;
}

ISR(TIMER4_COMPA_vect) {
    isr_counter++;
    leftLDR = adc_read(PIN_LDR_LEFT);
    rightLDR = adc_read(PIN_LDR_RIGHT);
    new_LDR_readings = true;
}

void seekBeacon(uint16_t leftLDR, uint16_t rightLDR) {
    //uint8_t motor_data[4];
    //int bug[5];

    // Calculate turn direction
    uint16_t total_magnitude = leftLDR + rightLDR;
    uint16_t turn = (total_magnitude == 0) ? 512 : 1024 - (1024 * leftLDR / total_magnitude);
    
    // calculate speed
    uint16_t speed;
    uint16_t avg_magnitude = total_magnitude / 2;

    if (avg_magnitude >= PROXIMITY_THRESHOLD) {
        speed = 512;  // Stop or idle speed
        // Display "found" on LCD
    } else {
        // Scale speed linearly with distance
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