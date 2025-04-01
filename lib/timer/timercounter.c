#include <avr/interrupt.h>

// TIMER 1 //

void timer1_init()
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

ISR(TIMER1_COMPA_vect) //Interrupt service routine (handler)
{
  // Serial.Println(“One second has passed)”;
  return;
}