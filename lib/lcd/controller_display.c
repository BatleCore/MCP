// controller display lib
#include "controller_display.h"

volatile ScreenState currentScreen = SCREEN_HOME;

void display_init() {
    lcd_init();
    DDRD &= ~(1 << PD0);
    PORTD |= (1 << PD0);
    // Enable INT0 on falling edge (button press)
    EIMSK |= (1 << INT0);             // Enable INT0
    EICRA |= (1 << ISC01);            // ISC01=1, ISC00=0 → falling edge
    EICRA &= ~(1 << ISC00);

    // Enable global interrupts
    sei();
}
void nextScreen() {
    currentScreen = (currentScreen + 1) % SCREEN_COUNT;
    lcd_clrscr();
}

void updateLCD() {

    uint8_t dataRX[6];
    char buffer[20];
    uint8_t bat_percentage = 0;

    lcd_goto(0x00); // line 1

    switch(currentScreen) {
        case SCREEN_HOME: {

            serial2_write_bytes(1, BATTERY_REQUEST);
            //packet = requestHOMEdata();
            if (serial2_available()) {
                serial2_get_data(dataRX, 2);

                if (dataRX[0] == BATTERY_REQUEST) {
                    bat_percentage = 100 * dataRX[1] / 84;

                    sprintf(buffer, "%s MODE: ", auto_mode ? "AUT" : "MAN");
                    lcd_puts(buffer);

                    lcd_goto(0x40);
                    sprintf(buffer, "BAT: %d.%dV %3d%% ", dataRX[1] / 10, dataRX[1] % 10, bat_percentage);
                    lcd_puts(buffer);
                }
            }
        break; }

        case SCREEN_LIGHT: {
        // packet = requestLDRdata()
        // parse packet and assemble with sprintf
        // lcd_puts
            serial2_write_bytes(1, LDR_REQUEST);
            //packet = requestHOMEdata();
            if (serial2_available()) {
                serial2_get_data(dataRX, 5);

                if (dataRX[0] == LDR_REQUEST) {
                    // LEFT:  0000 00hz
                    sprintf(/*above*/);
                    lcd_puts(buffer);

                    lcd_goto(0x40);
                    // RIGHT: 0000 00hz
                    sprintf(/*above*/);
                    lcd_puts(buffer);
                }
            }
        break; }

        case SCREEN_RANGE: {
            serial2_write_bytes(1, RANGE_REQUEST);
            //packet = requestHOMEdata();
            if (serial2_available()) {
                serial2_get_data(dataRX, 2);

                if (dataRX[0] == RANGE_REQUEST) {
                    bat_percentage = 100 * dataRX[1] / 84;

                    sprintf(buffer, "%s MODE: ", auto_mode ? "AUT" : "MAN");
                    lcd_puts(buffer);

                    lcd_goto(0x40);
                    sprintf(buffer, "BAT: %d.%dV %3d%% ", dataRX[1] / 10, dataRX[1] % 10, bat_percentage);
                    lcd_puts(buffer);
                }
            }
        break; }
    }
}

ISR(INT0_vect) {
    static uint32_t last_press = 0;
    if (milliseconds_now() - last_press > 250) {
        currentScreen = (currentScreen + 1) % SCREEN_COUNT;
        last_press = milliseconds_now();
    }
}

void test_LCD(ScreenState screen) {
    lcd_clrscr();
    lcd_goto(0x00);  // Start of first line

    switch (screen) {
        case SCREEN_HOME:
            lcd_puts("Home Screen");
            break;
        case SCREEN_LIGHT:
            lcd_puts("Light Sensors");
            break;
        case SCREEN_RANGE:
            lcd_puts("Range Sensors");
            break;
        case SCREEN_COUNT:
            // do nothing or display error
            break;
        default:
            lcd_puts("Unknown Screen");
            break;
    }
}