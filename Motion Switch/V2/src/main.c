#define F_CPU 8000000UL

#define LED1_INIT				pinModePB(1,OUTPUT)
#define LED1_ON					setZero(PORTB,1)
#define LED1_OFF				setPlus(PORTB,1)
#define LED1_TOGGLE				ibi(PORTB,1)

#include "main.h"
#include "Atmega8T0.h"
#include "sleep.h"
#include "INT0.h"
#include "comparator.h"
#include "adc.h"
//***************************************************************************
// Установки
uint8_t light = false;
uint8_t ch = 0;
//***************************************************************************
// Инициализация
void init(void)
{
	LED1_INIT;
}

//***************************************************************************
//Функция вызыается 10 раз в секунду или каждые 100мс
void ms100(void)
{

}

//Функция вызыается раз в секунду
void second(void)
{
	
}

int main(void)
{
	init();
	t0_init();
	//int0_init();
	sleep_init();
	//comparator_init();
	//adc_init();
	sei();
	
	sleep_enable();
	
	uint8_t i = 0;
	
	while(1){
		//Запускаем ADC
		//ADCSRA |=(1<<ADSC);
		//delay(650);
		//Проверка на наличие света
		//light = (ADCH < 0x03)?true:false;
		
		//if( light ){
			LED1_ON;
			
		//}else{
		//	LED1_OFF;
		//}
		
		delay(65000);
		
		if( i++ == 25 ){
			i = 0;
			LED1_OFF;
			sleep();
		}
	}
}
