#include <avr/interrupt.h>
#include <avr/io.h>

void second(void);

//Вектор при переполнении счетчика
ISR( TIMER0_OVF_vect )
{
	static uint16_t counter = 0;
	if(counter < 15625){
		counter++;
	}else{
		counter = 0;
		second();
	}
}
void t0_init()
{
	TCCR0|=(1<<CS00);//предделитель/1
	//TCCR0|=(1<<CS01);//предделитель/8
	//TCCR0|=(1<<CS01)|(1<<CS00);//предделитель/64
	//TCCR0|=(1<<CS02)|(1<<CS00);//предделитель/1024
	TIMSK|=(1<<TOIE0);		// Разрешаем прерывание по переполнению Т0
}
