#include "light_sensor.h"

/*************************************************
=== Variable Initialisations ===
*************************************************/
volatile uint32_t isr_counter = 0;
char msg[40];

volatile uint16_t leftLDR = 0;
volatile uint16_t rightLDR = 0;
volatile int16_t signalLeft = 0;
volatile int16_t signalRight = 0;
volatile uint16_t freqLeft = 0;
volatile uint16_t freqRight = 0;
volatile uint16_t baselineLeft = 0;
volatile uint16_t baselineRight = 0;
volatile int16_t signal_max[2] = {0};

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

  return (LDRval - (channel ? baselineRight : baselineLeft));

  // static uint16_t baseline[2] = {0};                          // Array storing left[0] and right [1] baseline light levels
  // baseline[channel] = (baseline[channel] * 99UL + LDRval) / 100;  // Update baseline using exponential moving average
  // return (int16_t)LDRval - (int16_t)baseline[channel];        // Return signed deviation from baseline
}

int16_t getBaseline(uint16_t LDRval, uint8_t channel) {
  static uint16_t baseline[2] = {0};                          // Array storing left[0] and right [1] baseline light levels
  baseline[channel] = ((baseline[channel] * 99UL + LDRval) / 100);  // Update baseline using exponential moving average
  baseline[channel] += (LDRval > baseline[channel])? 1 : -1; // compensate for integer rounding. ensures value doesnt stall on a rounding-down loop
  return baseline[channel]; 
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

    //static int16_t  last_signal[2]    = {0}; not needed but scared to delete - mat
    static uint32_t last_time[2]      = {0};
    static uint16_t last_freq[2]      = {0};
    static bool     new_edge[2]       = {0};
    static uint16_t edge_counter[2]   = {0};
    static bool     last_state[2]     = {false}; // false for low, true for high

    uint32_t now = milliseconds_now();
    if (!last_state[channel] && signal > SIGNAL_THRESHOLD) {
        // sprintf(msg, "\n%s hard max : %d", channel ? "right" : "left", signal);
        // serial0_print_string(msg);
        signal_max[channel] = signal;
        last_state[channel] = true;
        uint32_t dt = now - last_time[channel];
        last_time[channel] = now;
        if (dt > 47) {
            new_edge[channel] = true;
            edge_counter[channel] = 0;
            last_freq[channel] = (200000UL) / dt;
        }
    } else if (last_state[channel]) {
        if ( signal < SIGNAL_LOW_THRESHOLD ) {
            last_state[channel] = false;
        }
        if ( signal_max[channel] < signal ) {
            // sprintf(msg, "\n%s soft max : %d", channel ? "right" : "left", signal);
            // serial0_print_string(msg);
            signal_max[channel] = signal;
        } 
      }
    edge_counter[channel]++;
    if (edge_counter[channel] > 400 && !new_edge[channel]) {
        last_freq[channel] = 0;
    }
    new_edge[channel] = false;
    // last_signal[channel] = signal;

    if ( !last_freq[channel] ) {
        signal_max[channel] = 0;
    } 
    if ( signal_max[channel] > 1023 ) {
        signal_max[channel] = 0;
    }


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
    // adc_read(0); // a nothing operation - seems to be needed to prime the adc
    leftLDR = adc_read(PIN_LDR_LEFT); // callibration.txt
    rightLDR = adc_read(PIN_LDR_RIGHT);
    // Check for signal against ambient light level
    baselineLeft = getBaseline(leftLDR, 0);
    baselineRight = getBaseline(rightLDR, 1);

    signalLeft = getSignal(leftLDR, 0);
    signalRight = getSignal(rightLDR, 1);
    // Calculate potential beacon frequency
    freqLeft = getFrequency(signalLeft, 0);
    freqRight = getFrequency(signalRight, 1);
    // sprintf(msg, "\n(%lu, %d)", isr_counter, signalLeft);
    // serial0_print_string(msg);
    // sprintf(msg, "(\n%lu, %d)", isr_counter, signalRight);
    // serial0_print_string(msg);
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
void seekBeacon() {
        
    static int prep_counter = 0;
    static int leftVal = 0;
    static int rightVal = 0;
    static uint16_t distance_values[3] = {0};
    static int samples = 1;
    static float thresh = 1.2; // +- for going straight

    // sprintf(msg, "\nseekBeacon : %d", prep_counter);
    // serial0_print_string(msg);

    sprintf(msg, "\nL: %4d, R: %4d", freqLeft, freqRight);
    serial0_print_string(msg);
    // sprintf(msg, "\nL: %3d, R: %3d", signalLeft, signalRight);
    // serial0_print_string(msg);
    // sprintf(msg, "\nL: %3d, R: %3d", signal_max[0], signal_max[1]);
    // serial0_print_string(msg);
    // sprintf(msg, "\nL: %3d, R: %3d", leftVal, rightVal);
    // serial0_print_string(msg);

    get_distances(distance_values); // this is causing frequency errors?
    sprintf(msg, "\nL: %d, C: %d, R: %d", distance_values[0], distance_values[1], distance_values[2]);
    serial0_print_string(msg);

    if (prep_counter >= samples) {
        // seeking code
        leftVal = (leftVal * (samples - 1) + signal_max[0])/samples;
        rightVal = (rightVal * (samples - 1) + signal_max[1])/samples;


        if (distance_values[1] > FRONT_HARD_LIM) { // no wall in front
            if ( leftVal == 0 && rightVal == 0 ) { // no signals
                sprintf(msg, "\nspot %s - no signals", (distance_values[0]<distance_values[2]) ? "right" : "left");
                serial0_print_string(msg);
                motor_turn_spot(distance_values[0]<distance_values[2]); // rotate away from closest wall
            } else if ( leftVal == 0 || rightVal == 0 ) { // only one signal ( becomes XOR due to previous IF)
                // one value exists, not both
                // hard turn in that direction
                sprintf(msg, "\nhard %s - missing signal", (rightVal > leftVal) ? "right" : "left");
                serial0_print_string(msg);
                motor_hardturn_forward(rightVal > leftVal); // if leftVal == 0 , turn right
            } else { // both signals
                if ( leftVal > rightVal * thresh ) { // left too strong
                    // if left is much brighter
                    // go left
                    serial0_print_string("\nleft  - uneven signals");
                    motor_softturn_forward(0);
                } else if ( rightVal > leftVal * thresh ) { // right too strong
                    // if right is much brighter
                    // go right
                    serial0_print_string("\nright - uneven signals");
                    motor_softturn_forward(0);
                } else { // left and right about the same
                    // left and right are similar
                serial0_print_string("\nstraight - even signals");
                    motor_straight_forward();
                }
            }
        } else { // front obstruction
            serial0_print_string("\nstop - wall");
            motor_stop();
        }
    } else if (prep_counter == 0) {
        leftVal = signal_max[0];
        rightVal = signal_max[1];
        prep_counter++;
    } else {
        leftVal = (leftVal * (samples - 1) + signal_max[0])/samples;
        rightVal = (rightVal * (samples - 1) + signal_max[1])/samples;
        prep_counter++;
    }

    // sprintf(msg, "\nLmax: %3d : %3d : %3d\nRmax: %3d : %3d : %3d", leftVal, signal_max[0], signalLeft, rightVal, signal_max[1], signalRight);
    // serial0_print_string(msg);

    // Execute motor instruction
    
    // rs_motor_conversion();
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
void LDR_calibrate() {
  static char msg[40];
    leftLDR = 1 * adc_read(PIN_LDR_LEFT);
    rightLDR = adc_read(PIN_LDR_RIGHT);
    sprintf(msg, "(%d, m * %d + c )\n", rightLDR, leftLDR);
    serial0_print_string(msg);
}