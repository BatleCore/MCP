// Lab 3

// 1. Demonstrate display of formatted voltage to serial terminal.
// 2. Demonstrate using an interrupt to update display value
// 3. Demonstrate a system which measures the number of falling or rising edges within the last second.
// 4. Calculate and implement an RC filter on the interrupt pin to minimize the effect of button bounce.
// 5. Implement a software solution to the button bounce problem, discuss why each method of button debounce would be used.

//include this .c file's header file
#include "Lab.h"
#include "../lib/serial/serial.h"
#include <avr/interrupt.h>

//static function prototypes, functions only called in this file

int task = 1; // define which task to run in the switch-case systems
volatile int button_trigger = 0;
int trigger_counter = 0;
volatile uint32_t DB_timestamp = 0;
volatile uint32_t current_timestamp = 0;

const int bounce_ms = 50;

int main(void)
{

  // CONFIGURE INTERRUPTS
  DDRD &= ~(1<<PD0);// INT0 is also PD0 and we set the DDR to input
  PORTD |= (1<<PD0);// enable pullup resistor on PD0
  EICRA |= (1<<ISC01); // these two bits set
  EICRA &= ~(1<<ISC00);// INT0 to trigger on a FALLING edge
  EIMSK |= (1<<INT0); // enable INT0
  sei(); // activate interrupts globally

  task_loop();
  
  return(1);
}//end main 

int test(void)
{
  char message[] = "message printed how gooooood";
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
  int AVal = 0; // value read from analog pin
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
    // AVolt = ADC_voltage_calc(AVal);
    AVolt = ((5 * AVal )/ 1024.0) *1000;
    // format into string
    char rounded[10];
    char voltage_str[40];
    // sprintf(rounded, "%.2f", AVolt);
    // sprintf(voltage_str, "%s%s", rounded, " V\n");
    
    // 4226 == 4.226 
    // var1 = 4
    // var2 = 226 
    // int var1 = 4226/1000 // ==4 
    // int var = 4226 % (4*1000)

    // int intmVolt = AVolt * 1000;
    // int Vol1 = intmVolt / 1000;
    // int Vol2 = intmVolt % 6;

    int whole = AVolt;

    sprintf(voltage_str, "voltage %d mV\n", whole);

    // send to serial

    if ( serial0_available ) // bug check this condition
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
  // exclude time delay

  while(1) // loop
  {
    do
    {
      _delay_ms(50); // idle when not in use
    } while (button_trigger == 0);

    // read ADC and send via serial

    /* copy contense of task 1 while() loop here */
    
    button_trigger = 0; // reset the conditions for next loop
  }
  return(0);
}

int task_3(void)
{
  // 3. Demonstrate a system which measures the number of falling or rising edges within the last second.
  while(1)
  {
    _delay_ms(1000); // wait 1 second

    // send to serial
    if ( serial0_available() ) // bug check this condition
    {
      char message[10];
      sprintf(message, "%s", trigger_counter);

      serial0_print_string(message);
    }
    // send via serial
    button_trigger = 0; // reset the trigger

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

  // if ( ADC_value >= 1023) // clamp max limit
  // {
  //   ADC_value = 1023;
  // }
  // else if ( ADC_value <= 0) // clamp min limit
  // {
  //   ADC_value = 0;
  // }

  // float Volt_min = 1; // to be callibrated
  // float Volt_max = 4; // to be callibrated 
  // float Volt_range = Volt_max - Volt_min;

  // int ADC_min = 1023 * ( Volt_min / 5);
  // int ADC_max = 1023 * ( Volt_max / 5);
  // int ADC_range = ADC_max - ADC_min;

  // int percentage_result = ((ADC_value - ADC_min) * 100 ) / ADC_range; // result is from 0 to 100
  float voltage_result = 5 * (ADC_value / 1024);

  // return(percentage_result); // returns an int 
  return(voltage_result); // returns a float
}


// TASK 2 FUNCTIONS
// TASK 3 FUNCTIONS
// TASK 4 FUNCTIONS
// TASK 5 FUNCTIONS
// INTERRUPT CODE

ISR(INT0_vect)
{
  switch (task)
  {
  // case 0:
    /* UNUSED */
  //   break;

  // case 1:
    /* UNUSED */
  //   break;
  
  case 2:

    // not debouncing this one?

    // button debounce
    // button_trigger = 1;
    break;

  case 3:
    // not debouncing this one?
    trigger_counter++;

    break;

  case 4:
    // hardware debounce
    // low signal 0v > 1.5v
    // high signal 3c > 5v

    // DISCHARGING ( pressed )
    // V(cap) = V(i) * e^( -t / (R1) * C )

    // CHARGING ( released )
    // V(cap) = V(f) * ( 1 - e^( -t / (R1 + R2) * C ) )

    // Charging:
    // 1.5v, 20ms
    // 1.5 = 5 * ( 1 - e^( -0.02 / (R1 + R2) * C ) )
    // ( R1 + R2 ) * C = 0.05607

    // Discharging:
    // 5 > 3v, 1ms
    // 3 = 5 * e^( -0.001 / (R1) * C )
    // (R1) * C ) = 0.00196

    // R1 = R2 * 27.6

    // let C = 0.0000001 F ( 100 nF )
    // R2 = 20k
    // therefore R1 = 540k

    // let C = 0.000001 ( 1 uF )
    // R2 = 2k
    // therefore R1 = 54k

    // let C = 0.0000022 ( 2.2 uF )
    // R2 = 0k9
    // therefore R1 = 24k
    button_trigger = 1; // trigger without debounce...
    break;

  case 5:
    // software debounce
    // milliseconds_init();
    current_timestamp = milliseconds_now();
    if (current_timestamp - DB_timestamp > bounce_ms)
    {
      button_trigger = 1;
    }
    else
    {
      button_trigger = 0;
    }
    DB_timestamp = current_timestamp;
    
    break;
  }
}

int task_loop()
{
  switch (task)
  {
  case 0:
    test();
  break;

  case 1:
    task_1();
    break;
  
  case 2:
    task_2();
    break;

  case 3:
    task_3();
    break;

  case 4:
    task_4();
    break;

  case 5:
    task_5();
    break;
  }
  return(0);
}