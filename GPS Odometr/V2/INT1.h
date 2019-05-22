#include <avr/interrupt.h>
#include <avr/io.h>


uint8_t brightness = 7;

ISR(INT1_vect)
{
	if( mode == 4 ){
		if( brightness++ >= 7 ) brightness = 0;
		TM1637_setBrightness(brightness);
		eeprom_write( BRIGHTNESS_EEPROM_ADDR, brightness );
	}
}
//настройка внешнего прерывния INT1
void int1_init(void)
{
	pinModePD(3,INPUT);
	//настраиваем на Задний фронт на INT1
	MCUCR |=(1<<ISC11);
	MCUCR &=~(1<<ISC10);
	//разрешаем внешнее прерывание INT1 
	GICR |= (1<<INT1);
}
