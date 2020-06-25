#define F_CPU 8000000UL

#define LED1_INIT				pinModePB(1,OUTPUT)
#define LED1_ON					setZero(PORTB,1)
#define LED1_OFF				setPlus(PORTB,1)
#define LED1_TOGGLE				ibi(PORTB,1)

#define SLEEP_SEC_INIT			3

#include "main.h"
#include "Atmega8T0.h"
#include "sleep.h"
#include "INT0.h"
#include "adc.h"
//***************************************************************************
// Установки
uint8_t sleepSec = SLEEP_SEC_INIT;
//***************************************************************************
// Инициализация
void init(void)
{
	LED1_INIT;
}

//***************************************************************************
void int0_interrupt(void)
{
	sleepSec += 10;
}
//Функция вызыается 10 раз в секунду или каждые 100мс
void ms100(void)
{

}

//Функция вызыается раз в секунду
void second(void)
{
	if( sleepSec ) sleepSec--;
}

int main(void)
{
	init();
	t0_init();
	int0_init();
	sleep_init();
	adc_init();
	sleep_enable();
	sei();
	
	while(1){

		adc_run();
	
		if( !adc_isLight() ){
			LED1_ON;
		}else{
			sleepSec = 0;
		}
		
		if( !sleepSec ){
			LED1_OFF;
			t0_reset();
			sleep();
			sleepSec += 75;
		}
	}
}
