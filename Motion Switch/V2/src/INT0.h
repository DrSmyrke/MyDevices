#include <avr/interrupt.h>
#include <avr/io.h>

/*
Внешние прерывания в avr atmega8 настраиваются при помощи бит ISCxx регистра MCUCR.

Зависимость условия срабатывания внешнего прерывания INT0 от бит ISC0x регистра MCUCR в avr atmega8
 ISC01 	 ISC00 	Условие срабатывания
 0 	 0 	 Низкий уровень на INT0
 0 	 1 	 Любое изменение на INT0
 1 	 0 	 Задний фронт на INT0
 1 	 1 	 Передний фронт на INT0

Для внешнего прерывания INT1, настройка производиться так же, только используются биты ISC11 ISC10.
*/




void int0_interrupt(void);


//функция обработчик внешнего прерывания INT0
ISR( INT0_vect )
{
	int0_interrupt();
}
//настройка внешнего прерывния INT0
void int0_init(void)
{
	pinModePD(2,INPUT);
	//настраиваем на Задний фронт на INT0
	//MCUCR |=(1<<ISC01);
	//MCUCR &=~(1<<ISC00);
	//разрешаем внешнее прерывание INT0 
	GICR |= (1<<INT0);
	
	//настраиваем на Задний фронт на INT0
	setZero(MCUCR,ISC01);
	setZero(MCUCR,ISC00);
}
