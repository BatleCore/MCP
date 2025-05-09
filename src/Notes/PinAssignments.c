#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <util/delay.h>
#include <stdio.h>
// DIGITAL


void Digital_IO_EG(void) {
  // example for port A
  #define PORT_EG PORTA
  #define DDR_EG DDRA
  #define PIN_OUT_EG PA0
  #define PIN_IN_EG PA1
  
  DDR_EG |= (1<<PIN_OUT_EG); // HIGH for input
  DDR_EG &= ~(1<<PIN_IN_EG); // LOW for output
}

void Analog_IN_EG(void) {

}


int main(void) {
  Digital_IO_EG();
  
  return 0;
}