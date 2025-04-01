/*
Task:
  Develop a simple stop watch using the serial terminal to show the elapsed time, this is a broad scope but some requirements are outlined below.
    - You will need to set up a hardware to a suitable interval (0.1, 1 or 10 ms), and write a program to keep count of milliseconds, seconds and minutes.
    - You will need to calculate a prescaler and choose register configurations
    - As a minimum the stopwatch needs to have a start/stop button, a reset button and display time in a readable format.
*/

/*
  1. Show calculations and register configuration for the setup of the timer.
  2. Draw a flowchart describing the logic of the stopwatch (using separate flows for the interrupts)
  3. Demonstrate a working timer (using timer interrupts)
  4. 2 marks - Demonstrate a functional stopwatch which meets the requirements - 2 marks 
*/

/*
// STOPWATCH FEATURES
start/stop button

reset button

*/


// configure timer counter (increments / units / ms?)
// define stopwatch function
// timer/counter interrupt, calls stopwatch function

/*
first button press / clear stopwatch counter (seconds = 0, milliseconds = 0, etc...)
stopwatch function is called as per timer interrupts, stopwatch counter is incremented in the timer/counter interrupt loop

*/

#include "Controller.h"

// SETUP GLOBAL

#define LED_PIN 0

int main(void)
{
  // startup
  DDRA |= (1<<LED_PIN); // set PORTA pin 0 to output (HIGH), leave other pins alone
  PORTA = 0; //set all pins low
  PORTA &= ~(1<<LED_PIN); // set pin 0 LOW, leave other pins alone

  while(1)
  {
    PORTA = 0b00000001;
    _delay_ms(200);
    PORTA = 0b00000000;
    _delay_ms(200); 
    break;
  }
}