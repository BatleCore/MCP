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
uint16_t turn = 0;
uint16_t speed = 0;

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

void getLightValues(uint16_t* light_values) {
  light_values[0] = leftLDR;
  light_values[1] = rightLDR;
}

void getSignals(uint16_t* signal_values) {
  signal_values[0] = signalLeft;
  signal_values[1] = signalRight;
}

void getFrequencies(uint16_t* freq_values) {
  freq_values[0] = freqLeft;
  freq_values[1] = freqRight;
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
void seekBeacon(uint16_t leftLDR, uint16_t rightLDR, uint16_t leftSignal, uint16_t rightSignal) {
    uint8_t motor_data[4];
    
    uint16_t total_magnitude = leftLDR + rightLDR;
    uint16_t total_signal = leftSignal + rightSignal;

    // calculate turn and speed
    if (total_magnitude == 0) { // Prevent divide-by-zero just in case
        turn = 0;
        speed = 0;
    } else if (leftSignal< rightSignal) {
        // More light on the Right -> turn Right
        motor_data[3] = 1; // turn right
        motor_data[1] = 1; // forwards

        turn = 250 * leftSignal / total_signal;
        speed = 250 - (250 * rightLDR / 1023);
    } else {
        // More light on the RIGHT -> turn LEFT
        motor_data[3] = 0; // turn left
        motor_data[1] = 1; // forwards

        turn = 250 * rightSignal / total_magnitude;
        speed = 250 - (250 * leftLDR / 1023);
    }

    motor_data[2] = turn;
    motor_data[0] = speed;

    // Execute motor instruction
    
    rs_motor_conversion(motor_data);
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