#include <avr/pgmspace.h>
//#include <util/delay.h>
#include <avr/io.h>
//***************************************************************************
//
// Каждая нота кодируется 1 байтом:
// 0-4 бит - код тона ноты
// 5-7 бит - код длительности ноты
//  
// Нота с кодом 1 - до 1-й октавы, с кодом 2 - до# 1-й октавы, с кодом 3 - ре 1-й октавы
// и т.д. Всего 32 ноты.
// 
// Если 0-4 биты равны 0, то это означает паузу длительностью, задаваемой 
// битами 5-7.
//
// Конец мелодии обозначается нулем (0x00),
//
//
//***************************************************************************

//Таблицы частот нот и длительностей
unsigned int  notefreq[] PROGMEM = {0,4748,4480,4228,3992,3768,3556,3356,3168,2990,2822,2664,2514,2374,2240,2114,1996,1884,1778,1678,1584,1495,1411,1332,1257,1187,1120,1057,998,942,889,839,792};
unsigned int  pausedelay[] PROGMEM = {32,64,128,256,512,1024,2048,4096};
unsigned char melody[] PROGMEM = {0x7A,0x7A,0x7A,0x7A,0x45,0x7A,0x45,0x7A,0x45,0x00};
// Своя задержка, так как _delay_ms на вход принимает только константу
void delay_note (unsigned int d){
	unsigned int i;
	for (i=0; i< d; i++) delay(100);
}
void play(){
	if(!alarmPort){alarmPort=1;setPlus(PORTB,2);}
	bgl=0;
	TCCR1A=0x00;
	TCCR1B=0x09;
	//PORTB=0x00;
	DDRB  |=  _BV(PB1); // Включаем нужный пин на выход
	PORTB &= ~_BV(PB1); // Устанавливаем в 0
	unsigned char freqnote;		// Код ноты
	unsigned char delaynote;	// Код длительности
	unsigned char stmp=1;			// Временная переменная
	unsigned char  sch=0;
	//while(ch<215){
	while(stmp!=0){
		//stmp=eeprom_read(sch);
	//while((pgm_read_byte(&(melody[ch])))!=0){
		stmp=pgm_read_byte(&(melody[sch]));
		freqnote=stmp&0x1F;			// Код ноты
		delaynote=(stmp>>5)&0x07;	// Код длительности
		if(freqnote!=0){			// Если не пауза
			// включаем звук
			OCR1A=pgm_read_word(&(notefreq[freqnote]));
			TCCR1A=0x40;
		}
		delay_note(pgm_read_word(&(pausedelay[delaynote]))); // выдерживаем длительность ноты
		TCCR1A=0x00; 	//выключаем звук
		delay_note(8); 	//выдерживаем паузу
		sch++;
	}
	setZero(DDRB,1);
	setPlus(PORTB,1);
	//DDRB  |=  _BV(PB1); // Включаем нужный пин на выход
}