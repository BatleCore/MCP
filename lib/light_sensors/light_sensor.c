#include "light_sensor.h"


#define PIN_LDR_LEFT PA4
#define PIN_LDR_RIGHT PA5

#define SAMPLE_RATE 100     // 100 Hz
#define SAMPLE_PERIOD 10    // 100hz = 10 ms 
#define SIGNAL_THRESHOLD 20 // Adjust from testing

#define FREQ_TARGET 2000     // 20.00 Hz
#define FREQ_TOL    500      // ±5 Hz tolerance

#define PROXIMITY_THRESHOLD 900
int16_t last_signal = 0;

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
    baseline = (*baseline * 9 + adc) / 10;
    // Signal = deviation from baseline (high-pass effect)
    int16_t signal = adc - *baseline;
    // Detect a rising zero-crossing (negative → positive) that exceeds threshold
    if (last_signal < 0 && signal >= 0 && abs(signal) > SIGNAL_THRESHOLD) {
        freq = (uint32_t)100000 / SAMPLE_PERIOD;
        }
    // Store current signal for next zero-crossing detection
    last_signal = signal;
    // Return the frequency (0 if no new edge this sample)
    return freq;
}

char directionLeftLDR;
char directionRightLDR;

ISR(TIMER3_COMPB_vect) {
    uint16_t leftLDR = adc_read(PIN_LDR_LEFT);
    uint16_t rightLDR = adc_read(PIN_LDR_RIGHT);

    uint32_t freqLeftLDR = getFrequency(leftLDR, 0); // Channel 0
    uint32_t freqRightLDR = getFrequency(rightLDR, 1); // Channel 1
    // Seek beacon if target frequency found
    if (abs(freqLeftLDR - FREQ_TARGET) < FREQ_TOL &&
        abs(freqRightLDR - FREQ_TARGET) < FREQ_TOL) {
        seekBeacon(leftLDR, rightLDR);
    }

}

void seekBeacon(uint16_t leftLDR, uint16_t rightLDR) {
    
    uint8_t motor_data[4];
    int bug[5];

    // Calculate turn direction
    uint16_t total_magnitude = leftLDR + rightLDR;
    uint16_t turn = (total_magnitude == 0) ? 512 : 1024 - (1024 * leftLDR / total_magnitude);
    
    
    // calculate spee
    uint16_t speed;
    uint16_t avg_magnitude = total_magnitude / 2;
    if (avg_magnitude >= PROXIMITY_THRESHOLD) {
        speed = 512;  // Stop or idle speed
        // Display "found" on LCD
    } else {
        // Scale speed linearly with distance
        // Far = fast, Near = slow
        speed = 1023 - avg_magnitude;
    }
    // Execute motor instruction
    motor_data_conversion(speed, turn, motor_data);
    differential_PWM_v3(motor_data);
}


    