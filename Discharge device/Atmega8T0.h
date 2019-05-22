#include <avr/interrupt.h>
#include <avr/io.h>

void ms100(void);
void second(void);

//Вектор при переполнении счетчика
ISR( TIMER0_OVF_vect )
{
	static unsigned short counter = 0;
	static unsigned short counter2 = 0;
	if(counter2 < 3125){
		counter2++;
	}else{
		counter2 = 0;
		ms100();
	}
	if(counter < 31250){
		counter++;
	}else{
		//second
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
