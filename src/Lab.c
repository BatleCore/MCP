// Lab 4

// TASK 1: Blinking lamp with transistor
// TASK 2: Range sensor to trigger motor with flyback
// TASK 3: Gather sensor data

// 1. Drive 12v load via transistor ( func 1 )
// 2. Drive 12v inductive load with flyback protection ( func 2 )
// 3. Trigger motor via range sensor ( func 2 )
// 4. Range sensor calibration plan ( func 3 )
//    - data linearization? take incremented measurements.
// 5. Describe function of flyback protection
//    - explain current flow when motor is switched off
//    - include circuit diagram

//include this .c file's header file
#include "Controller.h"

// GLOBAL VARIABLES
int task = 3; // define which task to run in the switch-case systems

// Pin numbers
int load_pin = 0;
int sens_pin = 1;

int main(void)
{

  task_loop();

  return(1);
}//end main 

int task_1(void)
{
  // 1. Drive 12v load via transistor
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

  DDRA |= (1<<load_pin); // set PORTA pin 0 to output (HIGH), leave other pins alone
  PORTA &= ~(1<<load_pin); // set pin 0 LOW, leave other pins alone


  while(1)
  {
    for (int i = 0; i < 3; i++)
    {
      PORTA |= (1<<load_pin); // HIGH / ON
      _delay_ms(2000);
      PORTA &= ~(1<<load_pin); // LOW / OFF
      _delay_ms(1000);
    }
    _delay_ms(0); // pause cycle, a style choice
  }
  return(0);
}

int task_2(void)
{
// 2. Drive 12v inductive load with flyback protection
// 3. Trigger motor via range sensor
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
  DDRA |= (1<<load_pin); // set PORTA pin 0 to output (HIGH), leave other pins alone
  PORTA &= ~(1<<load_pin); // set pin 0 LOW, leave other pins alone

  // INPUT
  DDRA &= ~(1<<sens_pin); //put PORT1(APin) into input mode (LOW)
  adc_init(); // initialize ADC
  _delay_ms(20); // 20 millisecond delay to set ADC

  // Serial init
  serial0_init();

  char message[4]; //string to send via serial
  int prox_thresh = 512; // proximity threshold for distance sensor
  int prox_real; // ADC value from sensor

  while(1)
  {
   prox_real = adc_read(sens_pin); // read sensor value
   sprintf(message, "%d", prox_real); // convert sensor int to char*

   if ( serial0_available ) // bug check this condition
   {
     serial0_print_string(message);
   }

   if (prox_real < prox_thresh)
   {
    PORTA |= (1<<load_pin); // set motor HIGH
   }
   else
   {
    PORTA &= ~(1<<load_pin); // set motor LOW
   }
   _delay_ms(50); // pause loop before reset 
  }
  return(0);
}

int task_3(void)
{
// 4. Range sensor calibration plan

  // INITIALIZATION


  // INPUT
  DDRA &= ~(1<<sens_pin); //put PORT1(APin) into input mode (LOW)
  adc_init(); // initialize ADC
  _delay_ms(20); // 20 millisecond delay to set ADC

  // Serial init
  serial0_init();

  char message[4]; // string to send via serial
  int prox_real; // ADC value from sensor
  int prox_sum = 0;
  float prox_average; // proximity average
  int prox_average_int;
  int samples = 5; // number of samples to average

  while(1)
  {
    for ( int i = 0; i < samples; i++)
    {
      prox_real = adc_read(sens_pin);
      prox_sum = prox_sum + prox_real;
      _delay_ms(10);
    }
    prox_average = prox_sum/samples;
    prox_average_int = (int)prox_average; // convert to int for serial
    prox_sum = 0; // reset for next loop

    sprintf(message, "%d\n", prox_average_int);
  //  if ( serial0_available ) // bug check this condition
  //  {
    serial0_print_string(message);
  //  }
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

// 5. Describe function of flyback protection
//    - explain current flow when motor is switched off
//    - include circuit diagram

/*
DC motors use current through a coil to produce an electro-magnetic force, converting electrical energy into kinetic energy.
A fundamental property of a conductive coil is Inductance. Inductors resist change in current.

STEP BY STEP

- switched on - closed circuit - low effective resistance
- voltage across inductor produces current through inductor
- current increases over a small time, producing an increasing electro-magnetic field around inductor
- current and electro-magnetic field reach maximum and stabilize. energy is stored in the electromagnetic field

- switched off - open circuit - high effective resistance
- energy is no longer provided by the voltage source
- electro-magnetic field collapses, returning the energy to the inductor and temporarily sustaining the current
- a circuit with defined current and an arbitrarily high resistance produces an arbitrarliy high voltage ( OHMs law )

components, such as transistors, have maximum operating voltages.
The voltage spike across the inductor may exceed the maximum collector-emitter voltage for the transistor, destroying the component.

This inductor voltage spike is called the "Flyback effect".

A flyback diode can be used as a way to quickly short out this high voltage across the inductor,
discharging the energy that could otherwise cause damage.

*/

/*

Data Linearization

Data from ADC has an inverse relationship to distance.
  f(x) = ( 1 / x ); 
  where f(x) is ADC values and x is distance in the range 30mm to 600mm OR 0mm to 30mm
  (data shows a tipping point at 30mm, use-case is either above or below, but not both, as two measurements can give the same ADC reading)

  the data can be linearized as D(x) = 1 / f(x), which expresses as a linear function and can therefore be expressed in the form "mx + c"
  allowing the microcontoller to calculate distance with only multiplication and addition

0 : 1
10 : 388
20 : 515
30 : 627
40 : 553
50 : 452
60 : 396
70 : 346
80 : 305
90 : 270
100 : 243
125 : 198
150 : 165
175 : 140
200 : 121
225 : 110
250 : 98
275 : 90
300 : 82
325 : 74
350 : 69
375 : 65
400 : 60
450 : 50
500 : 44
550 : 39
550 : 35
600 : 30

*/