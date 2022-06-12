#include <avr/interrupt.h>
#include <avr/io.h>

uint8_t ms100Flag = 0;
uint8_t secondFlag = 0;


ISR( TIMER0_OVF_vect )
{
	static unsigned short counter = 0;
	static unsigned short counter2 = 0;
	if(counter2 < 3125){
		counter2++;
	}else{
		counter2 = 0;
		ms100Flag = 1;
	}
	if(counter < 31250){
		counter++;
	}else{
		//second
		counter = 0;
		secondFlag = 1;
	}
}

void t0_reset()
{
	TCNT0 = 0x00;
	ms100Flag = 0;
	secondFlag = 0;
}

void t0_init()
{
	TCCR0|=(1<<CS00);//предделитель/1
	TIMSK|=(1<<TOIE0);		// Разрешаем прерывание по переполнению Т0
	t0_reset();
}
