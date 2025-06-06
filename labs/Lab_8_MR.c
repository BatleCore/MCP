// Lab 8 Robot Micro

#include "Robot.h"

 

// Constants

#define PACKETSIZE 6  // Max serial packet size we expect

 

// H-Bridge & Motor Control

#define PORT_PWM PORTB

#define DDR_PWM DDRB

#define PIN_PWM_ML PB5 // D11 → OC1A (PWM output for left motor)

#define PIN_PWM_MR PB6 // D12 → OC1B (PWM output for right motor)

#define PORT_CONTROL PORTA

#define PIN_ML_F PA0 // D22 → Left motor forward

#define PIN_ML_R PA1 // D23 → Left motor reverse

#define PIN_MR_F PA2 // D24 → Right motor forward

#define PIN_MR_R PA3 // D25 → Right motor reverse

#define DUTY_LEFT OCR1A

#define DUTY_RIGHT OCR1B

 

// Communication Command Codes

#define LDR_REQUEST 0xA0     // Controller is asking for light sensor data

#define JOYSTICK_READ 0xA1   // Controller is sending joystick values

#define REQUEST_ERROR 0xEE   // For handling invalid codes

 

// Light Sensor ADC Channels

#define LEFT_LDR_PIN 0

#define RIGHT_LDR_PIN 1

 

/********************

Timer1 PWM Setup

Phase and Frequency Correct PWM Mode (mode 8)

Prescaler = 8, ICR1 = 2000 → 500Hz PWM frequency

Used for motor speed control

********************/

void timerPWM_init() {

    TCCR1A = (1<<COM1A1)|(1<<COM1B1);     // Non-inverting PWM on OC1A and OC1B

    TCCR1B = (1 << WGM13) | (1 << CS11);  // Mode 8: PWM Phase & Freq Correct, Prescaler = 8

    ICR1 = 2000;                          // Set TOP for 500Hz

    OCR1A = 0;

    OCR1B = 0;

    DDR_PWM |= (1 << PIN_PWM_ML) | (1 << PIN_PWM_MR); // Set PWM pins as output

}

 

// Scale raw LDR reading (0–1023) to 0–255

uint8_t LDRmap(int reading) {

  return reading >> 2;

}

 

/********************

Joystick → Motor Mapping

- Converts joystick (X/Y) into motor PWM duty cycle and direction

- Handles turning and direction flipping logic

********************/

void differential_PWM(uint8_t x, uint8_t y) {

    uint8_t leftRatio = 100;

    uint8_t rightRatio = 100;

    uint8_t speedRatio = 0;

    uint16_t leftPWM = 0;

    uint16_t rightPWM = 0;

 

    // Determine left/right scaling based on X-axis

    if (x < 127) { // Joystick pushed left

        leftRatio = 100 * x / 127;

        rightRatio = 100;

    }

    else if (x > 128) { // Joystick pushed right

        leftRatio = 100;

        rightRatio = 100 * (255 - x) / 127;

    }

 

    // Calculate motor speeds based on Y-axis

    if (y > 128) { // Forward

        speedRatio = 100 * (y - 128) / 127;

        leftPWM = 2 * speedRatio * leftRatio;

        rightPWM = 2 * speedRatio * rightRatio;

 

        // Set direction: forward

        PORT_CONTROL |= (1 << PIN_ML_F);

        PORT_CONTROL &= ~(1 << PIN_ML_R);

        PORT_CONTROL |= (1 << PIN_MR_F);

        PORT_CONTROL &= ~(1 << PIN_MR_R);

    }

    else if (y < 127) { // Backward

        speedRatio = 100 * y / 127;

        leftPWM = 2 * speedRatio * rightRatio;

        rightPWM = 2 * speedRatio * leftRatio;

 

        // Set direction: reverse

        PORT_CONTROL |= (1 << PIN_ML_R);

        PORT_CONTROL &= ~(1 << PIN_ML_F);

        PORT_CONTROL |= (1 << PIN_MR_R);

        PORT_CONTROL &= ~(1 << PIN_MR_F);

    }

 

    // Write final PWM duty cycles to Timer1

    OCR1A = leftPWM;

    OCR1B = rightPWM;

}

 

/********************

Robot Initialization

- Serial for debug (Serial0)

- Serial2 for XBee communication

- ADC for joystick & LDRs

- Timer for PWM

********************/

void setup() {

    cli();                   // Disable interrupts during setup

    serial0_init();          // Debug serial

    serial2_init();          // Xbee serial

    milliseconds_init();     // Millisecond timing

    timerPWM_init();         // Timer1 PWM setup

    _delay_ms(20);

    adc_init();              // Analog input (joystick + sensors)

    _delay_ms(20);

    sei();                   // Enable interrupts

}

 

/********************

Main Program Loop

- Waits for incoming packet over Serial2

- Responds based on command type:

    → JOYSTICK_READ: control motors

    → LDR_REQUEST: return light sensor values

********************/

int main(void) {

    setup();

 

    uint8_t dataRX[PACKETSIZE];

    int x_val = 0;

    int y_val = 0;

 

    while (1) {

        if (serial2_available()) {

            serial2_get_data(dataRX, PACKETSIZE);

 

            switch (dataRX[0]) {

                case LDR_REQUEST: {

                    // Read and respond with LDR values

                    int left_LDR_read = adc_read(LEFT_LDR_PIN);

                    int right_LDR_read = adc_read(RIGHT_LDR_PIN);

                    uint8_t left_light = LDRmap(left_LDR_read);

                    uint8_t right_light = LDRmap(right_LDR_read);

                    serial2_write_bytes(2, left_light, right_light);

                    break;

                }

 

                case JOYSTICK_READ: {

                    // Use joystick X/Y to update motor control

                    x_val = dataRX[1];

                    y_val = dataRX[2];

                    differential_PWM(x_val, y_val);

                    break;

                }

 

                default: {

                    // Unrecognized command

                    serial2_write_bytes(1, REQUEST_ERROR);

                    break;

                }

            }

        }

    }

 

    return 0;

}

 

// CONTROLLER

// Lab 8 Controller Micro

#include "Controller.h"

 

// Joystick Analog Pins (forgot how this works on controller)

#define PIN_JOYSTICKLEFT_X 0  // ADC0

#define PIN_JOYSTICKLEFT_Y 1  // ADC1

#define PIN_JOYSTICKRIGHT_X 2 // ADC2 (unused)

#define PIN_JOYSTICKRIGHT_Y 3 // ADC3 (unused)

 

// Communication Codes (matches the robot)

#define LDR_REQUEST    0xA0  // Request light sensor data from robot

#define JOYSTICK_READ  0xA1  // Send joystick X/Y to robot for motor control

#define REQUEST_ERROR  0xEE  // Error / fallback code (not used here)

 

// Convert 10-bit ADC (0–1023) to 8-bit value (0–255)

uint8_t mapADC(uint16_t adc_val) {

    return adc_val >> 2; // divide by 4

}

 

// System setup

// - Initializes communication (serial2 for Xbee)

// - Initializes ADC and millisecond timing

void setup() {

    cli();                   // Disable interrupts while setting up

    serial2_init();          // XBee serial communication

    milliseconds_init();     // For timing control

    _delay_ms(20);

    adc_init();              // Enable analog input (for joysticks)

    _delay_ms(20);

    sei();                   // Enable global interrupts

}

 

// Main control loop

// Reads joystick values and sends them to the robot every 20ms

int main(void) {

    setup();  // Init everything

 

    uint16_t left_x_val;     // Raw ADC reading for joystick X

    uint16_t left_y_val;     // Raw ADC reading for joystick Y

    uint32_t lastSend = 0;   // Last time a packet was sent

 

    while (1) {

        // Check if 20ms has passed since last send

        // This limits packet rate to 50Hz (fast, but not overwhelming)

        if (milliseconds_now() - lastSend >= 20) {

            lastSend = milliseconds_now();

 

            // Read left joystick (X = turn, Y = forward/backward)

            left_x_val = adc_read(PIN_JOYSTICKLEFT_X);

            left_y_val = adc_read(PIN_JOYSTICKLEFT_Y);

 

            // Send joystick data as a 3-byte command packet:

            // [Command Code, X value, Y value]

            serial2_write_bytes(3, JOYSTICK_READ, mapADC(left_x_val), mapADC(left_y_val));

        }

    }

 

    return 0;

}