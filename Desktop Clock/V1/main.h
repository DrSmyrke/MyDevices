//#define TASCII(numb) (numb+48)
//#define DIG(numb) (numb/10)
//#define DEC(numb) (numb%10)

#define true 1
#define false 0

#define setPlus(reg,bit) reg |= (1<<bit)
#define setZero(reg,bit) reg &= ~(1<<bit)
#define ibi(reg,bit) reg ^= (1<<bit)
#define CheckBit(reg,bit) (reg&(1<<bit))
#define nop() {asm("nop");}		// функция асемблера для задержки
void delay(int t){			// функция задержки
	int x;
	for (x=t;x>0;x--)nop (); 
}
/*
// Функция задержки в мкс
void _delay_us(unsigned char time_us){
	register unsigned char i;
	for(i = 0; i < time_us; i++){
		asm volatile(" PUSH  R0 ");
		asm volatile(" POP   R0 ");
	}
}
 
// Функция задержки в мс
void _delay_ms(unsigned int time_ms){
	register unsigned int i;
	for(i = 0; i < time_ms; i++){
		_delay_us(250);
		_delay_us(250);
		_delay_us(250);
		_delay_us(250);
	}
}
*/
