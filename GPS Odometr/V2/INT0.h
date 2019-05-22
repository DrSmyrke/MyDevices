#include <avr/interrupt.h>
#include <avr/io.h>



uint8_t mode = 1;
uint8_t modeCounter = 1;

ISR( INT0_vect )
{
	modeCounter = 5;
}
//настройка внешнего прерывния INT0
void int0_init(void)
{
	pinModePD(2,INPUT);
	//настраиваем на Задний фронт на INT0
	MCUCR |=(1<<ISC01);
	MCUCR &=~(1<<ISC00);
	//разрешаем внешнее прерывание INT0
	GICR |= (1<<INT0);
}
