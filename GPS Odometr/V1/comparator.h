#include <avr/interrupt.h>

//ISR( ANA_COMP_vect )
//{
	
//	ACSR |=(1<<ACI);
//}
void comparator_init()
{
	//pinModePD(6,INPUT);
	pinModePD(7,INPUT);

	ACSR |= ( 0 << ACD ) | ( 1 << ACBG );// | ( 1 << ACIE );

	ACSR &= ~(1<<ACIS1);
	ACSR &= ~(1<<ACIS0);
}
