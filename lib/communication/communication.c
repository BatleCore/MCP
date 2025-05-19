#include "communication.h"

void comms_init(void) {
    serial2_init();
}

void requestHOMEdata() {
    serial2_write_bytes(1, BATTERY_REQUEST, battery_voltage);
}

void sendHOMEdata() {}

void requestLDRdata() {}

void sendLDRdata() {}

void requestRANGEdata() {}

void sendRANGEdata() {}