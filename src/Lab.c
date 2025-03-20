// Lab 4

// TASK 1: Blinking lamp with transistor
// TASK 2: Range sensor to trigger motor with flyback
// TASK 3: Gather sensor data

// 1. Drive 12v load via transistor
// 2. Drive 12v inductive load with flyback protection
// 3. Trigger motor via range sensor
// 4. Range sensor calibration plan
//    - data linearization? take incremented measurements.
// 5. Describe function of flyback protection
//    - explain current flow when motor is switched off
//    - include circuit diagram

//include this .c file's header file
#include "Controller.h"

//static function prototypes, functions only called in this file

// GLOBAL VARIABLES
int task = 1; // define which task to run in the switch-case systems


int main(void)
{
  task_loop();

  return(1);
}//end main 

int task_1(void)
{
  // power the lamp
  /*
  hfe = ?; transistor gain
  iL = ?; lamp current
  Vb = ?; base voltage
  Vs = 5; signal voltage

  iBE = iL / hfe; base-emitter current 
  ir = iBE; resistor current
  Vr = Vs - Vb; resistor voltage

  R = ir / Vr; resistor value
  */

  // INITIALIZATION

  DDRA |= (1<<PA0); // set PORTA pin 0 to output (HIGH), leave other pins alone
  PORTA &= ~(1<<PA0); // set pin 0 LOW, leave other pins alone


  while(1)
  {
    for (int i = 0; i < 3; i++)
    {
      PORTA |= (1<<PA0); // HIGH / ON
      _delay_ms(100);
      PORTA &= ~(1<<PA0); // LOW / OFF
      _delay_ms(100);
    }
    _delay_ms(400); // pause cycle, a style choice
  }
  return(0);
}

int task_2(void)
{
  // power the motor
  /*
  hfe = ?; transistor gain
  iL = ?; motor current
  Vb = ?; base voltage
  Vs = 5; signal voltage

  iBE = iL / hfe; base-emitter current 
  ir = iBE; resistor current
  Vr = Vs - Vb; resistor voltage

  R = ir / Vr; resistor value
  */

  // INITIALIZATION

  // OUTPUT
  DDRA |= (1<<PA0); // set PORTA pin 0 to output (HIGH), leave other pins alone
  PORTA &= ~(1<<PA0); // set pin 0 LOW, leave other pins alone

  // INPUT
  int APin = 1; // PORTA1 Analog Pin
  DDRA &= ~(1<<APin); //put PORT1(APin) into input mode (LOW)
  adc_init(); // initialize ADC
  _delay_ms(20); // 20 millisecond delay to set ADC

  // Serial init
  serial0_init();

  char message[4]; //string to send via serial
  int prox_thresh = 512; // proximity threshold for distance sensor
  int prox_real; // ADC value from sensor

  while(1)
  {
   prox_real = adc_read(APin);
   sprintf(message, "%d", prox_real);
   if ( serial0_available ) // bug check this condition
   {
     serial0_print_string(message);
   }

   if (prox_real < prox_thresh)
   {
    PORTA |= (1<<PA0); // set motor HIGH
   }
   else
   {
    PORTA &= ~(1<<PA0); // set motor LOW
   }
   _delay_ms(50); // pause loop before reset 
  }
  return(0);
}

int task_3(void)
{
  // power the motor
  /*
  hfe = ?; transistor gain
  iL = ?; motor current
  Vb = ?; base voltage
  Vs = 5; signal voltage

  iBE = iL / hfe; base-emitter current 
  ir = iBE; resistor current
  Vr = Vs - Vb; resistor voltage

  R = ir / Vr; resistor value
  */

  // INITIALIZATION


  // INPUT
  int APin = 1; // PORTA1 Analog Pin
  DDRA &= ~(1<<APin); //put PORT1(APin) into input mode (LOW)
  adc_init(); // initialize ADC
  _delay_ms(20); // 20 millisecond delay to set ADC

  // Serial init
  serial0_init();

  char message[4]; //string to send via serial
  int prox_real; // ADC value from sensor
  float prox_average; // proximity average
  int samples = 5; // number of samples to average

  while(1)
  {
    for ( int i = 0; i < samples; i++)
    {
      prox_real = adc_read(APin);
      prox_average = prox_average + prox_real;
      _delay_ms(10);
    }
    prox_average = prox_average/samples;

   sprintf(message, "%d", prox_average);
   if ( serial0_available ) // bug check this condition
   {
     serial0_print_string(message);
   }
   _delay_ms(1000); // pause loop before reset 
  }
  return(0);
}


int task_4(void)
{
  while(1)
  {
    continue;
  }
  return(0);
}

int task_5(void)
{
  while(1)
  {
    continue;
  }
  return(0);
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
