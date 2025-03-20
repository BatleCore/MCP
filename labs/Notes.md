## Pin Assignment

  DDRA = 0xFF; // set PORTA to HIGH - "output" mode
  DDRA = 1; // same: all pins HIGH
  DDRA = 0b11111111; same: all pins HIGH

  PORTA |= (1<<PA0); // set port a pin 0 HIGH
  PORTA |= 0b00000001; // same as above, |= is OR gate to not change other pins

  PORTA &= ~(1<<PA1); // set pin 1 LOW
  PORTA &= 0b11111101; same as above, &= ~ is NAND gate to not change other pins


## Interrupts

    
  #include <avr/interrupt.h>

  /* initialization */
  DDRD &= ~(1<<PD0); // INT0 is also PD0 and we set the DDR to input
  PORTD |= (1<<PD0); // enable pullup resistor on PD0

  EICRA &= ~(1<<ISC00); // these two bits set INT0 to trigger on a FALLING edge ( 0, 1 ) 
  EICRA |= (1<<ISC01); // others include:
  // ( 0, 0 ) LOW signal generates interrupt
  // ( 0, 1 ) CHANGE in signal generates interrupt
  // ( 1, 0 ) FALLING signal generates interrupt
  // ( 1, 1 ) RISING signal generates interrupt

  EIMSK |= (1<<INT0); // enable INT0
  sei(); // activate interrupts globally

  /* Function */
  ISR(INT0_vect)
  {
    /* interrupt opperation */
  }



