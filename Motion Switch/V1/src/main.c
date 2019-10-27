#define F_CPU 8000000UL

#define LED_PIN_NUM 6
#define LED_PORT PORTB

#define INV_LED_PIN_NUM 1
#define INV_LED_PORT PORTB

#define POWER_LED_PIN_NUM 3
#define POWER_LED_PORT PORTB

#define RELAY_PIN_NUM 4
#define RELAY_PORT PORTD

#define INV_RELAY_PIN_NUM 5
#define INV_RELAY_PORT PORTB

//#define M_SENSOR_POWER_PIN_NUM 2
//#define M_SENSOR_POWER_PORT PORTD
#define M_SENSOR_DATA_PIN_NUM 3
#define M_SENSOR_DATA_PORT PORTD

#define DAY_NIGHT_PIN_NUM 0
#define DAY_NIGHT_PIN PINB
#define DAY_NIGHT_PORT PORTB

#define POWER_ON	0
#define POWER_OFF	1



//#define M_SENSOR_ON digitalWrite(M_SENSOR_POWER_PORT,M_SENSOR_POWER_PIN_NUM,POWER_ON)
//#define M_SENSOR_OFF digitalWrite(M_SENSOR_POWER_PORT,M_SENSOR_POWER_PIN_NUM,POWER_OFF)
#define LED_ON digitalWrite(LED_PORT,LED_PIN_NUM,POWER_ON)
#define LED_OFF digitalWrite(LED_PORT,LED_PIN_NUM,POWER_OFF)
#define INV_LED_ON digitalWrite(INV_LED_PORT,INV_LED_PIN_NUM,POWER_ON)
#define INV_LED_OFF digitalWrite(INV_LED_PORT,INV_LED_PIN_NUM,POWER_OFF)
#define POWER_LED_ON digitalWrite(POWER_LED_PORT,POWER_LED_PIN_NUM,POWER_ON)
#define POWER_LED_OFF digitalWrite(POWER_LED_PORT,POWER_LED_PIN_NUM,POWER_OFF)
#define RELAY_ON digitalWrite(RELAY_PORT,RELAY_PIN_NUM,POWER_ON)
#define RELAY_OFF digitalWrite(RELAY_PORT,RELAY_PIN_NUM,POWER_OFF)
#define INV_RELAY_ON digitalWrite(INV_RELAY_PORT,INV_RELAY_PIN_NUM,POWER_ON)
#define INV_RELAY_OFF digitalWrite(INV_RELAY_PORT,INV_RELAY_PIN_NUM,POWER_OFF)



#include "main.h"
#include "adc.h"
#include "uart.h"
#include "Atmega8T0.h"
#include "comparator.h"
#include "INT1.h"
//***************************************************************************
// Установки
//unsigned short 0..65536
//***************************************************************************
void start(void)
{
	POWER_LED_ON;delay2();
	LED_ON;delay2();
	INV_LED_ON;delay2();
	RELAY_ON;delay2();
	INV_RELAY_ON;delay2();
}
void reset(void)
{
	POWER_LED_OFF;
	LED_OFF;
	INV_LED_OFF;
	RELAY_OFF;
	INV_RELAY_OFF;
}
// Инициализация
void avr_init(void)
{
	pinModePB(POWER_LED_PIN_NUM,OUTPUT);
	pinModePB(LED_PIN_NUM,OUTPUT);
	pinModePB(INV_LED_PIN_NUM,OUTPUT);
	pinModePD(RELAY_PIN_NUM,OUTPUT);
	pinModePB(INV_RELAY_PIN_NUM,OUTPUT);
	//pinModePD(M_SENSOR_POWER_PIN_NUM,OUTPUT);
	pinModePD(M_SENSOR_DATA_PIN_NUM,INPUT);

	reset();
	start();
	reset();

	//M_SENSOR_ON;
}
//***************************************************************************
//Функция вызыается 10 раз в секунду или каждые 100мс
void ms100(void)
{
 	//Основной алгоритм
 	if( (light &&  dayMode) || (!light && !dayMode) ){
 		if(motion){
			RELAY_ON;
 			LED_ON;
			//Инверсия
			INV_LED_OFF;
			INV_RELAY_OFF;
 		}else{
			RELAY_OFF;
 			LED_OFF;
 			//Инверсия
			INV_LED_ON;
			INV_RELAY_ON;
 		}
 	}else{
		motion = 0;
		RELAY_OFF;
 		LED_OFF;
 		//Инверсия
		INV_LED_OFF;
		INV_RELAY_OFF;
	}
}
//Функция вызыается раз в секунду
void second(void)
{
	//Проверка на наличие действия днем или ночью
	dayMode = ( CheckBit(DAY_NIGHT_PIN,DAY_NIGHT_PIN_NUM) ) ? true : false ;
	//Проверка на наличие света
	light = (ADCH < 0x03)?true:false;

	if(motion > 0) motion--;
	
//	if(!light && dayMode || light && !dayMode){
//		M_SENSOR_OFF;
//		motion = 0;
//	}else{
//		M_SENSOR_ON;
//	}
	//uart_send(ADCH);
	//uart_send(light);
	//uart_send(motion);
	//uart_send(dayMode);

	//Запускаем ADC
	ADCSRA |=(1<<ADSC);
}
//***************************************************************************
int main (void)
{
	avr_init();
 	t0_init();
 	uart_init();
 	adc_init();
 	comparator_init();
	int1_init();
 	sei();

  	while(1){
		
  	}
}
