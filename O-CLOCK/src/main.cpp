//--------------------------------------------------------
#define F_CPU 8000000UL

#define TESTLED_INIT				pinModePD( 3, OUTPUT )
//--------------------------------------------------------
#include "Atmega8T0.h"
#include "uart.h"
#include "main.h"
//--------------------------------------------------------


int main(void)
{
	TESTLED_INIT;

	uart_init();
	t0_init();
	sei();

	uint8_t counter = 0;

	while( 1 ){
		if( secondFlag ){
			secondFlag = 0;

			uart_send( counter++ );
		}
	}

	return 0;
}