#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <util/delay.h>
#include <stdio.h>

/************************************************************************
Serial functions included for simulation on TinkerCad 
************************************************************************/
//Define USART constants for ATmega328, see ATmega328P datasheet, pg 145
#define USART_BAUDRATE 9600
#define F_CPU 16000000
#define BAUD_PRESCALE ((((F_CPU/16)+(USART_BAUDRATE/2))/(USART_BAUDRATE))-1)

/************************************************************************
Initialise USART 0
See ATmega328P datasheet for register descriptions, pg 159
Input: None
Output: None
************************************************************************/
void serial0_init(void)
{
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);     //Enable bits for transmit and recieve
	UCSR0C = (1<<UCSZ00)|(1<<UCSZ01);   //Use 8- bit character sizes
	UBRR0 = BAUD_PRESCALE;              //Load baud rate prescaler into register
}

/************************************************************************
Print string via USART 0
See ATmega328P datasheet for register descriptions, pg 159
Input:      string_pointer      char array      string to be printed to serial 0
Output:     None
************************************************************************/
void serial0_print_string(char * string_pointer) 
{
	while(*string_pointer)              //While not null character (end of string)
	{
		while((UCSR0A&(1<<UDRE0))==0){} //Wait for register empty flag
		UDR0 = *string_pointer;         //Send what's at the string pointer to serial data register
		string_pointer++;               //Increment string pointer to go to next letter in string
	}
}

//********************************************************************//
//*************************Comment Below Here*************************//
//********************************************************************//

#define STUDENT_ID 12345678

/***********************************************************************
Short Description
************************************************************************/
void ping_timer_init(void)
{
	cli(); 
    TCCR1A = 0;                     
	TCCR1B = (1<<WGM12)|(1<<WGM13);
	TCNT1 = 0;                      
	ICR1 = 65535;          
	TIMSK1 |= (1<<TOIE1);    
	TCCR1B |= (1<<CS10);          
	sei();                        
}

//PING))) Sensor Pins
#define PING_SENSOR_PIN PD2
#define PING_SENSOR_PORT PORTD
#define PING_SENSOR_DDR DDRD
#define PING_SENSOR_$$$1_COUNT 200*16-1
#define PING_SENSOR_$$$2_COUNT 5*16-1
#define PING_SENSOR_$$$3_COUNT (750+100)*16-1
#define PING_SENSOR_$$$4_CODE 65535
#define PING_SENSOR_$$$5_CODE 65534
#define CONVERTING_CONSTANT 1 //Calculate value for this

//Global variables for PING))) Sensor
volatile uint16_t pingMicros;
volatile uint32_t pingValue;
volatile bool newReading;

/***********************************************************************
Short Description
************************************************************************/
ISR(TIMER1_OVF_vect)
{
    if(!newReading) 
    {
      pingMicros += 4000;  
      if(pingMicros > 18500)
      {
          newReading = true;
          pingMicros = PING_SENSOR_$$$5_CODE;
      }
    }
}

/***********************************************************************
Short Description
************************************************************************/
ISR(TIMER1_COMPA_vect)
{
    if(OCR1A == PING_SENSOR_$$$1_COUNT)
    {
        PING_SENSOR_DDR |= (1<<PING_SENSOR_PIN);
        PING_SENSOR_PORT |= (1<<PING_SENSOR_PIN); 
        OCR1A = PING_SENSOR_$$$2_COUNT;
        TCNT1 = 0;
    }
    else if(OCR1A == PING_SENSOR_$$$2_COUNT)
    {
        PING_SENSOR_DDR &= ~(1<<PING_SENSOR_PIN);
        PING_SENSOR_PORT &= ~(1<<PING_SENSOR_PIN);
        OCR1A = PING_SENSOR_$$$3_COUNT;
        TCNT1 = 0;
        EICRA |= (1<<ISC01)|(1<<ISC00);
      	EIFR = (1<<INTF0);
        EIMSK |= (1<<INT0);
    }
    else if(OCR1A == PING_SENSOR_$$$3_COUNT)
    {
        pingMicros = PING_SENSOR_$$$4_CODE; 
        newReading = true;
    }    
}

/***********************************************************************
Short Description
************************************************************************/
ISR(INT0_vect)
{
    if(PIND & (1<<PING_SENSOR_PIN))
    {
        TCNT1 = 0;
        pingMicros = 0;
        TIMSK1 &= ~(1<<OCIE1A);
        EICRA &= ~(1<<ISC00);
        EICRA |= (1<<ISC01);
    }else
    {
        pingValue = (pingMicros + (TCNT1>>4))*((STUDENT_ID/7)%1000);
        newReading = true;
        EIMSK &= ~(1<<INT0);
    }
}

/***********************************************************************
Short Description
************************************************************************/
int main(void)
{
    serial0_init();
    ping_timer_init();
    ping_sensor_start();
    char serial_string[16] = {0};
    uint16_t distance = 0;

    while (1)
    {   
        if(newReading)
        {
            if (pingMicros == PING_SENSOR_$$$4_CODE)
            {
                serial0_print_string("No response\n");
            }
            else if (pingMicros == PING_SENSOR_$$$5_CODE)
            {
                serial0_print_string("Echo Pulse Error\n");
            }
            else
            {
                distance = CONVERTING_CONSTANT*pingValue;
              	if(distance < 3000)
                {
                	sprintf(serial_string,"%4u mm\n",distance);
                	serial0_print_string(serial_string);
                }
              	else
                {
                	serial0_print_string("Object too far\n");
                }
            }
            ping_sensor_start();
            newReading = false;
        }
    }
}

/***********************************************************************
Short Description
************************************************************************/
void ping_sensor_start(void)
{
    PING_SENSOR_DDR &= ~(1<<PING_SENSOR_PIN);
    PING_SENSOR_PORT &= ~(1<<PING_SENSOR_PIN);
    OCR1A = PING_SENSOR_$$$1_COUNT;
    TIMSK1 |= (1<<OCIE1A);
  	TCNT1 = 0;
}