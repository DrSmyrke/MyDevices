#include <avr/interrupt.h>
#include <avr/io.h>

#define MOTION_SEC 45
uint8_t motion = MOTION_SEC;
uint8_t dayMode = false;
uint8_t light = false;


ISR(INT1_vect)
{
	if( (light &&  dayMode) || (!light && !dayMode) ) motion = MOTION_SEC;
}
//настройка внешнего прерывния INT1
void int1_init(void)
{
	//настраиваем на Задний фронт на INT1
	MCUCR |=(1<<ISC10);
	MCUCR |=(1<<ISC11);

	//MCUCR &=~(1<<ISC10);
	//MCUCR &=~(1<<ISC11);
	//разрешаем внешнее прерывание INT1 
	GICR |= (1<<INT1);
}
