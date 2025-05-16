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

void updateLCD( bool auto_mode, float battery, uint8_t beacon_id, float beacon_freq,
                uint16_t ldr_left, float freq_left,
                uint16_t ldr_right, float freq_right,
                uint16_t dist_left, uint16_t dist_front, uint16_t dist_right) {
    char buffer[20];

    lcd_goto(0x00); // line 1
    switch(currentScreen) {
        case SCREEN_HOME: 
            //packet = requestHOMEdata();
            sprintf(buffer, "%s MODE", auto_mode ? "AUT" : "MAN");
            lcd_puts(buffer);
        
        break;
        case SCREEN_LIGHT:
        // packet = requestLDRdata()
        // parse packet and assemble with sprintf
        // lcd_puts

        break;
        case SCREEN_RANGE:

        break;
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

ISR(INT0_vect) {
    serial0_print_string("bounce press\n");
    static uint32_t last_press = 0;
    if (milliseconds_now() - last_press > 250) {
        serial0_print_string("valid press\n");
        currentScreen = (currentScreen + 1) % SCREEN_COUNT;
        last_press = milliseconds_now();
    }
}