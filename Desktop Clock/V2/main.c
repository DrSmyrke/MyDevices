#define F_CPU 4000000UL

#define LED_POWER_PORT		PORTB
#define LED_POWER_PIN		1
#define LED_POWER_INIT		pinModePB(LED_POWER_PIN,OUTPUT)
#define LED_POWER_ON		digitalWrite(LED_POWER_PORT,LED_POWER_PIN,0)
#define LED_POWER_OFF		digitalWrite(LED_POWER_PORT,LED_POWER_PIN,1)

#define M_SENSOR_DATA_PIN_NUM 3
#define M_SENSOR_POWER_PIN_NUM 2
#define M_SENSOR_POWER_PORT PORTD

#define POWER_ON	0
#define POWER_OFF	1

#define M_SENSOR_ON digitalWrite(M_SENSOR_POWER_PORT,M_SENSOR_POWER_PIN_NUM,POWER_ON)
#define M_SENSOR_OFF digitalWrite(M_SENSOR_POWER_PORT,M_SENSOR_POWER_PIN_NUM,POWER_OFF)

#include "main.h"
#include "myProto.h"
#include "uart.h"
#include "ds1302.h"
#include "comparator.h"
#include "INT1.h"
#include "T1637.h"
#include "Atmega8T0.h"

#include "myProto.c"
//***************************************************************************
// Установки
//uint8_t Sec = 0;
//uint8_t Min = 5;
//uint8_t Hour = 0;
//***************************************************************************
// Инициализация
void avr_init(void)
{
	LED_POWER_INIT;
	LED_POWER_OFF;
	delay(10000);
	LED_POWER_ON;delay2();
	delay(10000);
	pinModePD(M_SENSOR_DATA_PIN_NUM,INPUT);
	pinModePD(M_SENSOR_POWER_PIN_NUM,OUTPUT);
	M_SENSOR_ON;
	LED_POWER_OFF;
}
//***************************************************************************
//Функция вызыается раз в секунду
void second(void)
{
	_PointFlag = ( _PointFlag )? 0 : 1;

	if( motion ) motion--;

	switch( DateTime.Hour ){
		case 10: TM1637_setBrightness(1); break;
		case 11: TM1637_setBrightness(3); break;
		case 12: TM1637_setBrightness(5); break;
		case 13: TM1637_setBrightness(7); break;
		case 14: TM1637_setBrightness(7); break;
		case 15: TM1637_setBrightness(6); break;
		case 16: TM1637_setBrightness(5); break;
		case 17: TM1637_setBrightness(4); break;
		case 18: TM1637_setBrightness(3); break;
		case 19: TM1637_setBrightness(2); break;
		case 20: TM1637_setBrightness(1); break;
	}
}
//***************************************************************************
int main (void)
{
	avr_init();
 	comparator_init();
	t0_init();
	uart_init();
	int1_init();
 	sei();

	TM1637_init(D4056A);
	TM1637_setBrightness(0);

  	while(1){
		if( uart.writeTimeFlag ){
			uart.writeTimeFlag = false;
			DS1302_WriteDateTime();
		}
		if( uart.resetTimeFlag ){
			uart.resetTimeFlag = false;
			DS1302_ResetTime();
		}
		DS1302_ReadDateTime();
		if( !motion ){
			uint8_t DispData[] = { 0x7f, 0x7f, 0x7f, 0x7f };
			TM1637_display_all(DispData);
		}else{
			TM1637_display( 0, DateTime.Hour / 10 );
			TM1637_display( 1, DateTime.Hour % 10 );
			TM1637_display( 2, DateTime.Min / 10 );
			TM1637_display( 3, DateTime.Min % 10 );
		}
		delay(5000);
  	}
}
