// Lab 3

// 1. Demonstrate display of formatted voltage to serial terminal.
// 2. Demonstrate using an interrupt to update display value
// 3. Demonstrate a system which measures the number of falling or rising edges within the last second.
// 4. Calculate and implement an RC filter on the interrupt pin to minimize the effect of button bounce.
// 5. Implement a software solution to the button bounce problem, discuss why each method of button debounce would be used.

//include this .c file's header file
#include "Lab.h"
#include "../lib/serial/serial.h"
#include "../lib/timer/milliseconds.h"
#include <avr/interrupt.h>

//static function prototypes, functions only called in this file

int task = 6; // define which task to run in the switch-case systems
volatile int button_trigger = 0;
int trigger_counter = 0;
volatile uint32_t programStart_timestamp = 0;
volatile uint32_t prevTrigger_timestamp = 0;
volatile uint32_t currentTrigger_timestamp = 0;

const int bounce_ms = 125;
char message[40];
int bounce_delta = 0;
int prevDelta = 0;
int currDelta = 0;


int main(void)
{
  programStart_timestamp = milliseconds_now();
  prevTrigger_timestamp = programStart_timestamp;

  // CONFIGURE INTERRUPTS
  DDRD &= ~(1<<PD0); // INT0 is also PD0 and we set the DDR to input
  PORTD |= (1<<PD0); // enable pullup resistor on PD0
  EICRA |= (1<<ISC01); // these two bits set
  EICRA &= ~(1<<ISC00); // INT0 to trigger on a FALLING edge
  EIMSK |= (1<<INT0); // enable INT0
  sei(); // activate interrupts globally

  serial0_init();
  milliseconds_init();

  while(1)
  {
    _delay_ms(1000); // wait 1 second
    cli();
    sprintf(message, "%d\n", trigger_counter);
    serial0_print_string(message);
    trigger_counter = 0; // reset the trigger
    sei();
  }  
  return(1);
}//end main 

// INTERRUPT CODE

ISR(INT0_vect)
{
    cli();
    currentTrigger_timestamp = milliseconds_now();
    currDelta = currentTrigger_timestamp - prevTrigger_timestamp;
    
    if ( currDelta > prevDelta * 1.4 )
    {
      trigger_counter++; // valid trigger
    }
    
    prevDelta = currDelta;
    prevTrigger_timestamp = currentTrigger_timestamp;
    sei();
}
