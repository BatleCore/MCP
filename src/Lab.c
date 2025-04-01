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

#include <avr/interrupt.h>
#include "Controller.h"

// SETUP GLOBAL

#define LED_PIN 0
#define SS_PIN 1 // Start - Stop
#define RE_PIN 2 // Reset

// STATES //
int LED_state = 0;
int timer_active = 0;

// VALUES //
int millis_all = 0;
int seconds = 0;
int millis = 0;

// SERIAL DATA //
char message[20];

int main(void)
{
  // CONFIGURE OUTPUT PINS //  
  DDRA |= (1<<LED_PIN); // set PORTA pin 0 to output (HIGH), leave other pins alone
  PORTA = 0; //set all pins low
  PORTA &= ~(1<<LED_PIN); // set pin 0 LOW, leave other pins alone

  // CONFIGURE INPUT PINS //  
  // SS_PIN
  // RE_PIN
  timer1_init_1s();

  while(1)
  {
    if(message)
    {
      //send message via serial
      sprintf(message, "/0");
    }
    _delay_ms(10);
  }
}


// TIMER 1 //

void timer1_init_1ms()
{
  /*
  TOP = 65535 // 16 bit counter
  crystal = 16,000,000 // 16 MHz
  PRE = 64 // prescaler, settable
  T(top) = (TOP+1) * PRE// 16000000
  T(top) = 65536 * 64 / 16000000
  T(top) = 0.262144 // seconds

  Time = 0.001
  TOP = (Time * 16000000/PRE) - 1
  TOP = (0.001 * 16000000 / 64) - 1
  TOP = 249 // compare value
  */

  // Control register A
  TCCR1A = 0; //disable hardware output

  // Control register B
  TCCR1B = (1<<CS11)|(1<<CS10); // 00000011 sets the prescaler to 64

  OCR1A = 249; // load the compare register

  // compare interrupt
  TIMSK1 = (1<<OCIE1A); // 00000010 enables the timer
}

void timer1_init_1s()
{
  /*
  TOP = 65535 // 16 bit counter
  crystal = 16,000,000 // 16 MHz
  PRE = 256 // prescaler, settable
  T(top) = (TOP+1) * PRE// 16000000
  T(top) = 65536 * 256 / 16000000
  T(top) = 1.048576 // seconds

  Time = 1
  TOP = (Time * 16000000/PRE) - 1
  TOP = (1 * 16000000 / 256) - 1
  TOP = 62499 // compare value
  */

  // Control register A
  TCCR1A = 0; //disable hardware output

  // Control register B
  TCCR1B = (1<<CS12); // 00000100 sets the prescaler to 64

  OCR1A = 62499; // load the compare register

  // compare interrupt
  TIMSK1 = (1<<OCIE1A); // 00000010 enables the timer
  sei();
}

ISR(TIMER1_COMPA_vect) //Interrupt service routine (handler)
{
  cli(); // disable interrupts
  if (timer_active)
  {
    timer_count();
  }
  return;
  sei(); // enable interrupts
}

void timer_count()
{
  millis_all++;
  seconds = millis_all / 1000;
  millis = millis_all - seconds*1000;
}

void timer_reset()
{
  millis_all = 0;
  seconds = 0;
  millis = 0;
}


ISR(vector) // start-stop button
{
  // debounce code
    if (timer_active)
    {
      // disable timer
      timer_active = 0;
      // create message for serial
      sprintf(message, "PAUSED - %d:%d", seconds, millis);
    }
    else
    {
      // enable timer
      timer_active = 1;
      sprintf(message, "STARTED");
    }

}


IRS(vector) // reset button
{
  // debounce code
  time_reset();
  // create message for serial
  sprintf(message, "RESET - %d:%d", seconds, millis);

}