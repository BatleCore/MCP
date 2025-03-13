// Lab 3

// 1. Demonstrate display of formatted voltage to serial terminal.
// 2. Demonstrate using an interrupt to update display value
// 3. Demonstrate a system which measures the number of falling or rising edges within the last second.
// 4. Calculate and implement an RC filter on the interrupt pin to minimize the effect of button bounce.
// 5. Implement a software solution to the button bounce problem, discuss why each method of button debounce would be used.

//include this .c file's header file
#include "Lab.h"

//static function prototypes, functions only called in this file

int main(void)
{
//   task_1();
//   task_2();
//   task_3();
//   task_4();
//   task_5();
  
  return(1);
}//end main 

int task_1(void)
{
  // 1. Demonstrate display of formatted voltage to serial terminal.

  // Variables init
  int AVal; // value read from analog pin
  float AVolt; // AVal converted to voltage


  // Analog init
  int APin = 0; // PORTA0 Analog Pin
  DDRA &= ~(1<<APin); //put PORTA(APin) into input mode (LOW)
  adc_init(); // initialize ADC
  _delay_ms(20); // 20 millisecond delay to set ADC

  // Serial init
  serial0_init();

  while(1)
  {
    // Read ADC
    AVal = adc_read(APin);
    // convert to voltage
    AVolt = 0;

    // send to serial

    // _delay_ms(250);
  }
  return(0);
}

int task_2(void)
{
  // 2. Demonstrate using an interrupt to update display value
  while(1)
  {
    continue;
  }
  return(0);
}

int task_3(void)
{
  // 3. Demonstrate a system which measures the number of falling or rising edges within the last second.
  while(1)
  {
    continue;
  }
  return(0);
}

int task_4(void)
{
  // 4. Calculate and implement an RC filter on the interrupt pin to minimize the effect of button bounce.
  while(1)
  {
    continue;
  }
  return(0);
}

int task_5(void)
{
  // 5. Implement a software solution to the button bounce problem, discuss why each method of button debounce would be used.
  while(1)
  {
    continue;
  }
  return(0);
}
