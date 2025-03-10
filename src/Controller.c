//Example ATmega2560 Project
//File: ATmega2560Project.c
//An example file for second year mechatronics project

//include this .c file's header file
#include "Controller.h"
#include "../../lib/adc/adc.h" //minimal adc lib
#include <stdint.h>


//static function prototypes, functions only called in this file

int main(void)
{
  // INIT SECTION

  // define input pin IDs
  int Px = 0; // PORTA0 Analog Pin
  int Py = 1; // PORTA1 Analog Pin
  int Sw = 2; // PORTA2 Digital Pin

  // define pin modes
  DDRC = 0xFF; //put PORTC into output mode
  PORTC = 0; // set PORTC pins LOW
  // PORTC |= (1 << Sw);

  DDRA = 0x00; //put PORTA into input mode
  PORTA |= (1<<PA2); // pull up resistor
  
  adc_init();
  _delay_ms(20); // 20 millisecond delay to set ADC


  // values
  int AVal = 0; // default analog read val
  int sw = 0; //default switch read val
  int sel = 1; // selected ADC input default


  // LOOP SECTION
  while(1) //main loop
  {

    if(~(PINA & (1<<PINA2))) //if PINA0 is high
    {
      _delay_ms(10);
      if(~(PINA & (1<<PINA2))){
        sel = sel ^ 1;
      }
    }

    // read switch
    PORTC = (PORTA & ~(1 << PC2)) | (sel << PC2);
    
    // read ADC
    AVal = adc_read(sel); // reads the pin defined by "input_toggle()"

    // set pins
    set_LEDs(AVal);

    _delay_ms(20); //20 millisecond delay to regulate cycle


  }
  
  return(1);
}//end main 


int input_toggle(int sw, int sel){
  if (sw == 1){
    sel ^ 1;    
  }
  return(sel);
}

int set_LEDs(int AVal){
  // 0 <= AVal <= 1023

  // define LED combinations
  uint8_t LED_signals[9] = {
    
    // 0b00000000,  // [0]
    0b00000001,  // [1]
    0b00000010,  // [2]
    0b00000100,  // [3]
    0b00001000,  // [4]
    0b00010000,  // [5]
    0b00100000,  // [6]
    0b01000000,  // [7]
    // 0b10000000  // [8]
  };
  if(AVal>715){
    AVal = 715;
  }
  if(AVal<0){
    AVal=0;
  }

  // convert ADC value into list index values
  int AVal_scaled = (AVal * 9) / 1024;

  //set output LEDs according to analog value
  PORTC = LED_signals[AVal_scaled];

  return(0);
}

// bool readButton() {
//   if (())
// }