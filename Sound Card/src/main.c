#define F_CPU 8000000UL
#include <avr/io.h>
#include "main.h"
#include "sounder.h"
#include "eeprom.h"

//***************************************************************************
// Установки

#define CMP_PIN_NUM			4
#define CMP_PORT			PORTD
#define CMP_PIN				PIND
#define CMP_INIT			pinModePD(CMP_PIN_NUM,INPUT)
#define CMP_DEINIT			pinModePD(CMP_PIN_NUM,OUTPUT)
#define CMP_READ			CheckBit(CMP_PIN,CMP_PIN_NUM)
#define CMP_ON				setZero(CMP_PORT,CMP_PIN_NUM)
#define CMP_OFF				setPlus(CMP_PORT,CMP_PIN_NUM)
#define CMP_TOGGLE			ibi(CMP_PORT,CMP_PIN_NUM)

#define MELODY_COUNT		4
#define MELODY_MEM_ADDR		0

uint8_t melody				= 0;
//***************************************************************************

// Инициализация
void init(void)
{
	melody = eeprom_read( MELODY_MEM_ADDR );
	
	CMP_INIT;
	
	uint8_t i;
	
	if( !CMP_READ ){
		for( i = 0; i < 100; i++ ) delay(500);
		if( !CMP_READ ){
			melody++;
			if( melody > MELODY_COUNT ) melody = 0;
			eeprom_write( MELODY_MEM_ADDR, melody );
		}
	}
	
	if( melody > MELODY_COUNT ) melody = 0;
	
	CMP_DEINIT;
}
//***************************************************************************

int main(void)
{
	init();
	sounder_init();
	
	while(1){
		switch( melody ){
			case 0: play( gravityFallse, sizeof(gravityFallse) ); break;
			case 1: play( taxi, sizeof(taxi) ); break;
			case 2: play( dgentelmeniUdachi, sizeof(dgentelmeniUdachi) ); break;
			case 3: play( starWars, sizeof(starWars) ); break;
			case 4: play( caribianPirats, sizeof(caribianPirats) ); break;
			default: play( gravityFallse, sizeof(gravityFallse) ); break;
		}
		//play( gravityFallse, sizeof(gravityFallse) );
		//play( taxi, sizeof(taxi) );
		//play( dgentelmeniUdachi, sizeof(dgentelmeniUdachi) );
		//play( caribianPirats, sizeof(caribianPirats) );
		//play( starWars, sizeof(starWars) );
	}
}
