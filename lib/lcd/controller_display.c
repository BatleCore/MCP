// controller display lib
#include "controller_display.h"

#define auto_mode 0 // placeholder for compiling - update controller main

volatile ScreenState currentScreen = SCREEN_HOME;
uint8_t dataRX[6];
char buffer[20];

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
    lcd_clrscr();
    currentScreen = (currentScreen + 1) % SCREEN_COUNT;
}

void updateHomeScreen () {

    requestBATTERYdata(); // Ask robot to send battery data

    // Listen for response w/ 200ms timeout
    uint32_t start_wait = milliseconds_now(); 
    while (!serial2_available()) {
        if (milliseconds_now() - start_wait >= 200) {
            sprintf(buffer, "%s MODE: ", auto_mode ? "AUT" : "MAN"); // Print first line - Operation Mode
            lcd_goto(0x40);
            lcd_puts(buffer);
            lcd_goto(0x40); 
            lcd_puts("BAT: TIMEOUT"); // Print second line - Battery Request Timeout
            return;
        }
    }

    // Response received
    serial2_get_data(dataRX, 2); // Get the 2-byte packet ([0] instruction code, [1] battery voltage)
    if (dataRX[0] == BATTERY_REQUEST) {
        uint8_t bat_percentage = (dataRX[1] > 70) ? (100 * (dataRX[1] - 70) / 14) : 0; // calculate battery percentage
        sprintf(buffer, "%s MODE: ", auto_mode ? "AUT" : "MAN"); // Print first line - Operation Mode
        lcd_goto(0x00);
        lcd_puts(buffer);
        sprintf(buffer, "BAT: %d.%dV %3d%% ", dataRX[1] / 10, dataRX[1] % 10, bat_percentage); // Print second line - Battery voltage & percentage
        lcd_goto(0x40);
        lcd_puts(buffer);
    }
}

void updateLightScreen() {

    requestLIGHTdata(); // Ask robot for light data

    // Wait for robot response
    uint32_t start_wait = milliseconds_now();
    while (!serial2_available()) {
        if (milliseconds_now() - start_wait >= 200) {
            lcd_puts("LDR: TIMEOUT"); // Response timeout print error
            return;
        }
    }

    // Response received
    serial2_get_data(dataRX, 5); // Get the 5-byte packet ([0] instruction code, [1,2] L/R LDR values, [3,4] L/R frequency values)
    if (dataRX[0] == LIGHT_REQUEST) {
        // get percentages in 2 digits
        uint8_t left_percentage = 100*dataRX[1]/250;
        uint8_t right_percentage = 100*dataRX[2]/250;
        if (left_percentage > 99) left_percentage = 99;
        if (right_percentage > 99) right_percentage = 99;


        // Format LEFT line: percentage and frequency
        if (dataRX[3] >= 10) { // no decimal needed
            sprintf(buffer, "LEFT: %02u%% %2uHz", left_percentage, dataRX[3] / 10);
        } else { // decimal needed
            sprintf(buffer, "LEFT: %02u%% .%1uHz", left_percentage, dataRX[3]);
        }
        lcd_goto(0x00);
        lcd_puts(buffer);

        // Format RIGHT line: percentage and frequency
        if (dataRX[4] >= 10) { // no decimal needed
            sprintf(buffer, "RIGHT: %02u%% %2uHz", right_percentage, dataRX[4] / 10);
        } else { // decimal needed
            sprintf(buffer, "RIGHT: %02u%% .%1uHz", right_percentage, dataRX[4]);
        }
        lcd_goto(0x40);
        lcd_puts(buffer);
    }
}

void updateRangeScreen() {

    requestRANGEdata();

    // Wait for robot response
    uint32_t start_wait = milliseconds_now();
    while (!serial2_available()) {
        if (milliseconds_now() - start_wait >= 200) {
            lcd_puts("RANGE: TIMEOUT"); // Response timeout print error
            return;
        }
    }    

    // Response received
    serial2_get_data(dataRX, 4); // Get the 4-byte packet ([0] instruction code, [1,2,3] L/C/R distance values)
    if (dataRX[0] == RANGE_REQUEST) {
        uint16_t leftDist = dataRX[1]*530UL/250 + 70; // values between 70 and 600
        uint16_t centDist = dataRX[2]*530UL/250 + 70;
        uint16_t rightDist = dataRX[3]*530UL/250 + 70;

        // Top line
        lcd_goto(0x00);
        lcd_puts("LEFT CENT RIGH  ");
        // Second line
        sprintf(buffer, "%3u  %3u  %3u mm", leftDist, centDist, rightDist);
        lcd_goto(0x40);
        lcd_puts(buffer);
        
    }

}

void updateLCD() {

    switch(currentScreen) {
        case SCREEN_HOME:
            updateHomeScreen();
            break;
        
        case SCREEN_LIGHT:
            updateLightScreen();
            break;

        case SCREEN_RANGE:
            updateRangeScreen();
            break;
        
        case SCREEN_COUNT:
            // Not real case - structure exists to remove compile warning
            break;
            
        default:
            lcd_goto(0x00);
            lcd_puts("LCD ERROR");
            break;
    }
}

// ISR for screen switch button (right joystick)
ISR(INT0_vect) {
    // Software debouncing
    static uint32_t last_press = 0;
    if (milliseconds_now() - last_press > 250) {
        currentScreen = (currentScreen + 1) % SCREEN_COUNT; // cycle through screens
        last_press = milliseconds_now();
    }
}

// For testing scrolling funcitonality
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