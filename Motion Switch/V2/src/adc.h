#include <avr/interrupt.h>
#include <avr/io.h>

void adc_init(){
	pinModePC(5,INPUT); 
	ADMUX = 0b00000101;
	ADMUX |= (1<<REFS0);
	ADCSRA |= (1<<ADEN)|(1<<ADPS0)|(1<<ADPS1)|(1<<ADPS2);
}