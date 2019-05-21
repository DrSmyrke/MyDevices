#define F_CPU 8000000UL
#include <avr/io.h>
#include "main.h"
#include "sounder.h"



void init(void)
{
	pinModePB( 1, OUTPUT );
	setFreq( 0 );
	TCCR0 = (1<<CS02);
}

int main(void)
{
	init();
	while(1){
		//play( gravityFallse, sizeof(gravityFallse) );
		//play( taxi, sizeof(taxi) );
		//play( dgentelmeniUdachi, sizeof(dgentelmeniUdachi) );
		//play( caribianPirats, sizeof(caribianPirats) );
		//play( starWars, sizeof(starWars) );
	}
}
