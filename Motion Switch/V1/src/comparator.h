#include <avr/interrupt.h>

ISR( ANA_COMP_vect )
{
	digitalWrite(POWER_LED_PORT,POWER_LED_PIN_NUM,!CheckBit(ACSR,ACO));
	ACSR |=(1<<ACI);
}
void comparator_init()
{
	//pinModePD(6,INPUT);
	pinModePD(7,INPUT);
	ACSR |= ( 0 << ACD ) | ( 1 << ACBG ) | ( 1 << ACIE );
}
