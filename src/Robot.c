//Example ATmega2560 Project
//File: ATmega2560Project.c
//An example file for second year mechatronics project

//include this .c file's header file
#include "Robot.h"

//static function prototypes, functions only called in this file

int main(void)
{
  DDRC = 0xFF;//put PORTC into output mode
  PORTC = 0; 
  while(1)//main loop
  {
    _delay_ms(500);     //500 millisecond delay
    PORTC |= (1<<PC2);  // note here PA3 is just an alias for the number 3
                        // this line is equivalent to PORTA = PORTA | 0b00001000   which writes a HIGH to pin 3 of PORTA
    _delay_ms(500); 
    PORTC &= ~(1<<PC2); // this line is equivalent to PORTA = PORTA & (0b11110111)  which writes a HIGH to pin 3 of PORTA
  }
  return(1);
}//end main 