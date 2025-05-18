#include "light_sensor.h"

/*************************************************
=== Variable Initialisations ===
*************************************************/
volatile uint32_t isr_counter = 0;

volatile uint16_t leftLDR = 0;
volatile uint16_t rightLDR = 0;
volatile int16_t signalLeft = 0;
volatile int16_t signalRight = 0;
volatile uint16_t freqLeft = 0;
volatile uint16_t freqRight = 0;
uint16_t turn = 512;
uint16_t speed = 512;

/*************************************************
=== Light Sensor Initilisation ===

- milliseconds for frequency detection
- Timer 4 in CTC mode, prescale 8
- ISR Compare Flag A at 5ms/200hz
*************************************************/

void LDR_init() {
    milliseconds_init();
    TCCR4A = 0;
    TCCR4B = (1<<WGM42)|(1<<CS41); // CTC Mode Prescale 8
    OCR4A = 9999; // 200hz ISR
    TIMSK4 |= (1<< OCIE4A); //enable the ISR
    sei(); // enable global ISRs
}

/*************************************************
=== Signal Detection Function ===

- Calculates a dynamic baseline of ambient light 
  levels using an exponential moving average
- Signal is difference to this baseline
- positive signal = brighter, negative signal = 
  darker

Inputs: 10 bit LDR reading
        channel (0 left LDR, 1 right LDR)
Outputs: uint16 signal
*************************************************/
int16_t getSignal(uint16_t LDRval, uint8_t channel) {

    static uint16_t baseline[2] = {0};                          // Array storing left[0] and right [1] baseline light levels
    baseline[channel] = (baseline[channel] * 9 + LDRval) / 10;  // Update baseline using exponential moving average
    return (int16_t)LDRval - (int16_t)baseline[channel];        // Return signed deviation from baseline
}

/*************************************************
=== Frequency Calculation Function ===

- Takes 2 signals, one current, one previous, as 
  well as the timestamps
- if last was negative, and current is positive, 
  zero crossing occured
- i.e., a light was present and turned off (negative 
  last) and has turned on again (positive current)

Inputs: int16 signal
        channel (0 left LDR, 1 right LDR)
Outputs: uint16 frequency
*************************************************/

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

/*************************************************
=== Timer4 Compare Flag A Interrupt ===

- Triggers at 200hz rate (Every 5ms)
- Reads LDR's, calculates a signal relative to 
  ambient baseline and calculates any potetial 
  beacon frequencies
Inputs: None

Outputs: left/right LDR values
         left/right frequencies (as extern 
         accessible variables)
*************************************************/
ISR(TIMER4_COMPA_vect) {
    isr_counter++; // used for debugging (serial print every 10 ISRs to not overflood serial)
    // Get sensor values
    leftLDR = adc_read(PIN_LDR_LEFT);
    rightLDR = adc_read(PIN_LDR_RIGHT);
    // Check for signal against ambient light level
    signalLeft = getSignal(leftLDR, 0);
    signalRight = getSignal(rightLDR, 1);
    // Calculate potential beacon frequency
    freqLeft = getFrequency(signalLeft, 0);
    freqRight = getFrequency(signalRight, 1);
}

/*************************************************
=== Autonomous Beacon Seeking Function ===

- Called when target beacon frequency is detected 
  (handled in main)
- Calculates a turn and speed based on relative 
  strengths of left/right LDR values
- Executes motor instructions based on turn/speed 
  values

Inputs: left and right LDR values
Outputs: None
*************************************************/
void seekBeacon(uint16_t leftLDR, uint16_t rightLDR) {
    //uint8_t motor_data[4];
    //int bug[5];

    // Calculate turn direction
    uint16_t total_magnitude = leftLDR + rightLDR;
    turn = (total_magnitude == 0) ? 512 : (1024 * leftLDR) / (total_magnitude);
    
    // calculate turn and speed
    if (total_magnitude == 0) { // Prevent divide-by-zero just in case
        turn = 512;
        speed = 512;
    } else if (leftLDR > rightLDR) {
        // More light on the LEFT -> turn RIGHT -> turn > 512
        turn = 512 + (512UL * (leftLDR - rightLDR)) / total_magnitude;
        speed = 1024 - (512 * rightLDR / 1024);
    } else {
        // More light on the RIGHT -> turn LEFT -> turn < 512
        turn = 512 - (512UL * (rightLDR - leftLDR)) / total_magnitude;
        speed = 1024 - (512 * rightLDR / 1024);
    }

    // Execute motor instruction
    //motor_data_conversion(speed, turn, motor_data, bug);
    //differential_PWM_v3(motor_data);
}

/*************************************************
=== Test Function for LDR's ===
- subject to change based on debugging
*************************************************/
void LDR_test() {
    serial0_init();
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