#include <avr/interrupt.h>
#include <avr/io.h>

//--- VARIABLES -----------------------------------------------------------------------------
uint16_t vcc3				= 0;
uint16_t vcc5				= 0;

//--- FUNCTIONS -----------------------------------------------------------------------------
void adc_init()
{
	pinModePC( 3, INPUT );
	pinModePC( 5, INPUT );
	// ADMUX = 0b00000101;
	ADMUX = 0b00000011;
	ADMUX |= (1<<REFS1);
	ADMUX |= (1<<REFS0);
	ADCSRA = 0;
	ADCSRA |= (1<<ADEN)|(1<<ADPS2);//|(1<<ADPS0);//|(1<<ADIE);
}

//--------------------------------------------------------------------------------------------
void adc_readData()
{
	if( CheckBit( ADMUX, MUX2 ) ){
		vcc5 = ( ADCH << 8 ) | ( ADCL );
	}else{
		vcc3 = ( ADCH << 8 ) | ( ADCL );
	}
}

//--------------------------------------------------------------------------------------------
void adc_run()
{
	// if( CheckBit( ADMUX, MUX2 ) ){
	// 	setZero( ADMUX, MUX2 );
	// 	setZero( ADMUX, MUX1 );
	// 	setZero( ADMUX, MUX0 );
	// }else{
	// 	setPlus( ADMUX, MUX2 );
	// 	setZero( ADMUX, MUX1 );
	// 	setPlus( ADMUX, MUX0 );
	// }

	//Запускаем ADC
	ADCSRA |= (1<<ADSC);
	//Ждем окончания преобразования
	if( !CheckBit( ADCSRA, ADIE ) ){
		while((ADCSRA & (1<<ADSC)));
		adc_readData();
	}
}

//--------------------------------------------------------------------------------------------
ISR( ADC_vect )
{
	//vcc = ADCW;

	adc_readData();
}

//--------------------------------------------------------------------------------------------
uint8_t adc_isLight()
{
	//Проверка на наличие света
	return ( ADC > 230 ) ? 0x01 : 0x00;
}
