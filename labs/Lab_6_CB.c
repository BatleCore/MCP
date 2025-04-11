/*

**TASK 1**
  Establish control over two servos, using two independent PWM signals produced by one or two of the microcontroller timers.

### Outcomes
  1. Show calculations for PWM base frequency and duty cycle, state clearly the sage operating range for comparison values
  2. Explain how the timer/counter registers are configured to achieve the desired PWM
  3. Demonstrate an appropriate PWM on an oscilloscope
    - Demonstrate this prior to connecting to a servo motor, an incorrect waveform can damage the servo motors
  4. Demonstrate *smooth* control of a servo motor using a joystick
  5. Demonstrate simultaneous independent control of two servo motors
    - This may require a 5 volt regulator if the power requirements for the servo motors is too large
*/

/*
## SERVO SPECS

  voltage: 4-6v DC
  current: 100mA moving no load
  mapping:
    620us  = 0 deg   - 0.62ms
    1520us = 90 deg  - 1.52ms
    2420us = 180 deg - 2.42ms
  freq: 50Hz - 20ms - 20,000us
*/

/*
## BASE FREQ CALC
clock = 16,000,000
TOP = 2^N ( N = counter bits )
TOP = 65536 ( timer 1 )
f = clock / (2 * TOP * PRE)

f = 16000000 / ( 2 * 65536 * PRE) = 50 Hz
PRE = 16000000 / ( 2 * 65536 * 50 )
PRE = 

currently:
  PRE: 64
  Mode: Phase correct
  TOP: 2^n = 256
  PRE: 64
  f = clock / ( 2 * TOP * PRE )

implementing:
  n = 9
  TOP = 2^9 = 512
  PRE = 256
  f target = 50
  f = clock / ( 2 * 512 * 256)
  f = 60Hz ( 61.03515625 )
  period = 0.016384
*/

/*
## duty period for servo pulse
  cycle period = 0.016384
  increment period (incp)= 0.000032
  servo min (smin)= 0.62ms = 0.00062
  smin/incp = 19
  servo max ( smax ) = 2.42ms = 0.00242
  smax/incp = 74
  74 - 19 = 55 positions
*/

#include <avr/interrupt.h>
#include "Controller.h"

#define servo_1_pin DDB5 // digital pin 11
#define servo_2_pin DDB6 // digital pin 12

int main(void)
{
  DDRB |= (1<<servo_1_pin); // set servo 1 pin to output
  DDRB |= (1<<servo_2_pin); // set servo 2 pin to output

  // Disable Timer 1 - set all LOW
  TCCR1A = 0;
  TCCR1B = 0;

  // Set phase correct 9-bit (512)
  // Lecture 6, page 17
  TCCR1A |= (1 << WGM11);  // Set phase correct PWM, 9-bit mode

  // Set mode: toggle on compare match, clear on bottom/overflow
  // Lecture 6, page 18
  TCCR1A |= (1 << COM1A0) | (1 << COM1A1);  // Toggle OC1A on compare match

  // Set clock prescaler to 256 (for approximately 60Hz base frequency)
  // Lecture 6, page 19
  TCCR1B |= (1 << CS12);  // Prescaler = 256

  // TOP = 512
  OCR1A = 0;
  OCR1B = 0;
  
  sei();

  int duty_1A = 256;
  int duty_1B = 128;

  while (1)
  {
    

    OCR1A = duty_1A;
    OCR1B = duty_1B;
    

    duty_1A = (duty_1A == 256) ? 512 : 256;
    duty_1B = (duty_1B == 128) ? 300 : 128;

    _delay_ms(1000);
  }

  return 0;
}


/*
Duty cycle calc
Timer config
base freq
wave gen mode
inverting or non inverting


Timer 1 used, 3 OCRs
  OCR1A
  OCR1B
  OCR1C

desired base freq of 50Hz (20ms) as per data sheet

TCCR1A:
  WGM10, WGM11, WGM13 set to high, WGM12 set LOW, wave gen mode 11. 
  Phase correct PWM with TOP set by OCR1A - 16-bit timer

  clock = 16,000,000
  f = 50
  Tmax = 2^16 = 65535
  f = clock / ( 2 * TOP * PRE)
  TOP = clock / ( 2 * 50 * PRE)
  if PRE is 1 and f is 50, TOP needs to be 160,000. Larger than Tmax
  
  if PRE is 8 and f is 50, TOP needs to be 20,000. Less than Tmax, therefore valid.

  TCCR1B |= (1<<CS11) // 010 PRE set to 8
  OCR1A = 20000 // compare set to cycle every 20ms

  two outputs from the same timer: 1B, 1C
  set to compare match - non inverting mode

  for COM1B: 1 0
  TCCR1A &= ~(1<<COM1B0) // set LOW
  TCCR1A |= (1<<COM1B1) // set HIGH

  for COM1C: 1 0
  TCCR1A &= ~(1<<COM1C0) // set LOW
  TCCR1A |= (1<<COM1C1) // set HIGH

  ## Setting Duty Cycle

  cycle range: 920 to 1970 us ( constrained to avoid motor limits )
  0.92ms to 1.97 ms

  20ms period with TOP of 20,000

  duty min = 4.6%
  duty max = 9.9%

  COM_val = TOP*min = 20000*0.046 = 920
  COM_val = TOP*max = 20000*0.099 = 1970

  COM1B will output through PB6
  COM1C will output through PB7



*/