#include "communication.h"

char msg[40];

void comms_init() {
    serial2_init();
}

// done
void requestBATTERYdata() {
    serial2_write_bytes(1, BATTERY_REQUEST);
}

// done - checked and working
void sendBATTERYdata() {
    // Sends ADC reading of battery level as 8 bit
    uint8_t batteryVoltage = getVoltage();
    serial2_write_bytes(2, BATTERY_REQUEST, batteryVoltage);
}

// Controller
void requestLIGHTdata() {
    serial2_write_bytes(1, LIGHT_REQUEST);
    serial0_print_string("\nrequest sent");
}

void sendLIGHTdata() {
    // Get raw data
    uint16_t lightValues[2];
    uint16_t frequencies[2];
    getLightValues(lightValues);
    getFrequencies(frequencies);
    // Compress data to within 8 bit for sending
    uint8_t leftLDR = compressADC(lightValues[0]);
    uint8_t rightLDR = compressADC(lightValues[1]);
    uint8_t leftFreq = (frequencies[0] + 5) / 10; // Truncates least significant digit with nearest 10 rounding
    uint8_t rightFreq = (frequencies[1] + 5) / 10;
    // Sending data
    serial2_write_bytes(5, LIGHT_REQUEST, leftLDR, rightLDR, leftFreq, rightFreq); 
}

void requestRANGEdata() {
    serial2_write_bytes(1, RANGE_REQUEST);
}

void sendRANGEdata() {
    // Get raw data
    uint16_t rangedata[3];
    get_distances(rangedata);
    // Compress data for sending
    uint8_t leftDist = compressADC(rangedata[0]);
    uint8_t centDist = compressADC(rangedata[1]);
    uint8_t rightDist = compressADC(rangedata[2]);
    serial2_write_bytes(4,RANGE_REQUEST, leftDist, centDist, rightDist);
}

// Controller
void sendMotorControl() {
    uint8_t motor_toSerial[4];
    cs_motor_conversion(motor_toSerial);
    serial2_write_bytes(5, MOTOR_CONTROL, motor_toSerial[0], motor_toSerial[1], motor_toSerial[2], motor_toSerial[3]);
    // sprintf(msg, "0: %d, 1: %d\n2: %d, 3: %d\n\n", motor_toSerial[0], motor_toSerial[1], motor_toSerial[2], motor_toSerial[3]);
    // serial0_print_string(msg);
}

// Controller
void sendServoControl() {
    uint8_t servoControl[2];
    getServoControl(servoControl);
    serial2_write_bytes(3, SERVO_CONTROL, servoControl[0], servoControl[1]);
    // sprintf(msg, "0: %d, 1: %d\n", servoControl[0], servoControl[1]);
    // serial0_print_string(msg);
}

// Controller
void sendSwitchOperation(uint8_t opMode) {
    serial2_write_bytes(2, MODE_SWITCH, opMode);
}

// compresses 10 bit to 0-250 for serial
uint8_t compressADC(uint16_t adr_value) {
    return (uint8_t)((adr_value * 250UL) / 1023);
}

