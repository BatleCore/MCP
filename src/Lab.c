// Lab 3

// 1. Demonstrate display of formatted voltage to serial terminal.
// 2. Demonstrate using an interrupt to update display value
// 3. Demonstrate a system which measures the number of falling or rising edges within the last second.
// 4. Calculate and implement an RC filter on the interrupt pin to minimize the effect of button bounce.
// 5. Implement a software solution to the button bounce problem, discuss why each method of button debounce would be used.

//include this .c file's header file
#include "Lab.h"
#include "../lib/serial/serial.h"

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

int test(void)
{
  char message[] = "message printed";
  serial0_init();
  if (serial0_available){
    serial0_print_string(message);
  }
  return(0);
}

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
    AVolt = ADC_voltage_calc(AVal);

    // format into string
    char rounded[10];
    char voltage_str[10];
    sprintf(rounded, "%.2f", AVolt);
    sprintf(voltage_str, "%s%s", rounded, " V");

    // send to serial

    if ( serial0_available() ) // bug check this condition
    {
      serial0_print_string(voltage_str);
    }

    _delay_ms(250);
  }
  return(0);
}

int task_2(void)
{
  // 2. Demonstrate using an interrupt to update display value

  // copy Task 1 init stage here once it is working

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

// TASK 1 FUNCTIONS
float ADC_voltage_calc(int ADC_value){

  if ( ADC_value >= 1023) // clamp max limit
  {
    ADC_value = 1023;
  }
  else if ( ADC_value <= 0) // clamp min limit
  {
    ADC_value = 0;
  }

  float Volt_min = 1; // to be callibrated
  float Volt_max = 4; // to be callibrated 
  float Volt_range = Volt_max - Volt_min;

  int ADC_min = 1023 * ( Volt_min / 5);
  int ADC_max = 1023 * ( Volt_max / 5);
  int ADC_range = ADC_max - ADC_min;

  int percentage_result = ((ADC_value - ADC_min) * 100 ) / ADC_range; // result is from 0 to 100
  float voltage_result = 5 * (ADC_value / 1023);

  // return(percentage_result); // returns an int 
  return(voltage_result); // returns a float
}


// TASK 2 FUNCTIONS


// TASK 3 FUNCTIONS


// TASK 4 FUNCTIONS


// TASK 5 FUNCTIONS

