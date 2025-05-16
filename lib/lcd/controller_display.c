#include "controller_display.h"

typedef enum {
    SCREEN_HOME = 0,
    SCREEN_LIGHT,
    SCREEN_RANGE,
    SCREEN_COUNT
} LCD_Screen;

volatile currentScreen = SCREEN_HOME;

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
            packet = requestHOMEdata()
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