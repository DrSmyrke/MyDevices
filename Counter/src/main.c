#define F_CPU 8000000UL

#define RESET_BUTTON_INIT		pinModePC(5,INPUT)
#define ONE_BUTTON_INIT			pinModePB(1,INPUT)
#define TEN_BUTTON_INIT			pinModePB(5,INPUT)
#define HUNDERD_BUTTON_INIT		pinModePC(2,INPUT)
#define RESET_BUTTON_READ		CheckBit(PINC,5)
#define ONE_BUTTON_READ			CheckBit(PINB,1)
#define TEN_BUTTON_READ			CheckBit(PINB,5)
#define HUNDERD_BUTTON_READ		CheckBit(PINC,2)

#define DEC_BUTTON_INIT		pinModePC(4,INPUT)
#define DEC_BUTTON_READ		CheckBit(PINC,4)

#include "eeprom.h"
#include "main.h"
#include "T1637.h"
#include "Atmega8T0.h"
//***************************************************************************
// Установки
uint16_t value = 0;
uint16_t targetValue = 0;
uint8_t save = false;
uint8_t reset = false;
uint8_t rstButF = false;
uint8_t oneButF = false;
uint8_t tenButF = false;
uint8_t hundButF = false;

uint8_t decButF = false;
//***************************************************************************
// Инициализация
void init(void)
{	
	RESET_BUTTON_INIT;
	ONE_BUTTON_INIT;
	TEN_BUTTON_INIT;
	HUNDERD_BUTTON_INIT;
	
	DEC_BUTTON_INIT;
	
	value = eeprom_read( 0 )<<8;
	value += eeprom_read( 1 );
	
	if( value > 9999 ) value = 9999;
	
	TM1637_init(D4036B);				//D4036B
	TM1637_setBrightness( 4 );
	_PointFlag = 1;
}

void saveCounter(void)
{
	eeprom_write( 0, value>>8 );
	eeprom_write( 1, value );
}

//***************************************************************************
//Функция вызыается 10 раз в секунду или каждые 100мс
void ms100(void)
{
	if( reset && value > 0 ){
		if( value > 1000 ){
			value -= 1000;
		}else{
			if( value > 100 ){
				value -= 100;
			}else{
				if( value > 10 ){
					value -= 10;
				}else{
					value--;
				}
			}
		}
		
		if( value == 0 ){
			reset = false;
			save = true;
		}
	}
	
	if( reset && value == 0 ) reset = false;
	
	if( targetValue > 0 ){
		if( targetValue > 1000 ){
			targetValue -= 1000;
			value += 1000;
		}else{
			if( targetValue > 100 ){
				targetValue -= 100;
				value += 100;
			}else{
				if( targetValue > 10 ){
					targetValue -= 10;
					value += 10;
				}else{
					targetValue--;
					value++;
				}
			}
		}
		
		if( value > 9999 ){
			value = 9999;
			targetValue = 0;
		}
		
		if( targetValue == 0 ) save = true;
	}
	
	TM1637_display_int_decimal(value,true);
}

//Функция вызыается раз в секунду
void second(void)
{
	
}

int main(void)
{
	init();
	t0_init();
	sei();
	
	while(1){
		
		if( !reset ){
			if( ONE_BUTTON_READ && !oneButF ){
				delay(16000);
				if( ONE_BUTTON_READ && !oneButF ){
					targetValue++;
					oneButF = true;
				}
			}
			if( !ONE_BUTTON_READ && oneButF ){
				delay(16000);
				if( !ONE_BUTTON_READ && oneButF ) oneButF = false;
			}
			//////////////////////////////////////////////////////////////
			if( TEN_BUTTON_READ && !tenButF ){
				delay(16000);
				if( TEN_BUTTON_READ && !tenButF ){
					targetValue += 10;
					tenButF = true;
				}
			}
			if( !TEN_BUTTON_READ && tenButF ){
				delay(16000);
				if( !TEN_BUTTON_READ && tenButF ) tenButF = false;
			}
			//////////////////////////////////////////////////////////////
			if( HUNDERD_BUTTON_READ && !hundButF ){
				delay(16000);
				if( HUNDERD_BUTTON_READ && !hundButF ){
					targetValue += 100;
					hundButF = true;
				}
			}
			if( !HUNDERD_BUTTON_READ && hundButF ){
				delay(16000);
				if( !HUNDERD_BUTTON_READ && hundButF ) hundButF = false;
			}
			//////////////////////////////////////////////////////////////
			if( RESET_BUTTON_READ && !rstButF ){
				delay(16000);
				if( RESET_BUTTON_READ && !rstButF ){
					reset = true;
					rstButF = true;
				}
			}
			if( !RESET_BUTTON_READ && rstButF ){
				delay(16000);
				if( !RESET_BUTTON_READ && rstButF ) rstButF = false;
			}
			//////////////////////////////////////////////////////////////
			if( DEC_BUTTON_READ && !decButF ){
				delay(16000);
				if( DEC_BUTTON_READ && !decButF ){
					if( value > 0 ) value--;
					decButF = true;
				}
			}
			if( !DEC_BUTTON_READ && decButF ){
				delay(16000);
				if( !DEC_BUTTON_READ && decButF ) decButF = false;
			}
		}
		
		if( save ){
			save = false;
			saveCounter();
		}
	}
}
