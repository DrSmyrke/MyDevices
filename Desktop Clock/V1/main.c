#define F_CPU 4000000
//#define F_CPU 4000000UL
unsigned char Sec,Min,Hour,Day,Month,Year,DayOfWeek,AMPM,H12_24;
unsigned char menu,mode,buff[16],alarmNum,alarmNumB,ch,bgl,alarmPort;
//0 .. 255
//unsigned int timer;
//0 .. 65535
#include "main.h"
#include "ds1302.h"
#include "lcd5110.c"
#include "eeprom.h"
#include "sounder.h"
#include "Atmega8T0.h"
//***************************************************************************
// Установки
#define BMODE 6
#define BSET 7
#define DDR_BUTTON DDRB
#define PORT_BUTTON PORTB
#define PIN_BUTTON PINB
//***************************************************************************
// Инициализация
void avr_init(){
	menu=false;
	mode=0;
	bgl=30;
	// Начальная установкzа времени
	Sec=1;
	Min=13;
	Hour=23;
	Day=17;
	Month=5;
	Year=16;
	DayOfWeek=5;
	AMPM = AM;   //AM/PM
	H12_24 = H24;  //H12/H24
	//Кнопки
	DDR_BUTTON |= (0 << BMODE)|(0 << BSET); // Кнопки
	PORT_BUTTON |= (1 << BMODE)|(1 << BSET); // Подключаем подтягивающие резисторы
}
//***************************************************************************
void drawMenu(){
	unsigned char atmp;
	delay(500);
	if(menu==0){
		lcd_clear();
		delay(1000);
		lcd_clear();
		lcd_goto_xy(1,1);lcd_str("==============");
		lcd_goto_xy(1,6);lcd_str("==============");
		menu++;
		mode=0;
	}
	if(menu==1){
		lcd_goto_xy(1,2);lcd_str("   SET TIME                 ");
		sprintf(buff, "   %02i:%02i:%02i  ",Hour,Min,Sec);lcd_str(buff);
		if(mode==0){lcd_str("              ");}
		if(mode==1){lcd_str("    ^       ");}
		if(mode==2){lcd_str("       ^    ");}
		if(mode==3){lcd_str("          ^ ");}
	}
	if(menu==2){
		lcd_goto_xy(1,2);lcd_str("   SET DATE                ");
		sprintf(buff, " %02i.%02i.20%2i ",Day,Month,Year);lcd_str(buff);
		if(mode==0){lcd_str("               ");}
		if(mode==1){lcd_str("    ^     ");}
		if(mode==2){lcd_str("       ^  ");}
		if(mode==3){lcd_str("           ^");}
		if(mode==4){lcd_str("               ^");}
		lcd_chrWD(DayOfWeek,74,4);
	}
	if(menu==3){
		lcd_goto_xy(1,2);lcd_str("  SET ALARM   ");
		sprintf(buff, "%i %02i:%02i ",alarmNum/3,eeprom_read(alarmNum+1),eeprom_read(alarmNum+2));lcd_str(buff);
		atmp=eeprom_read(alarmNum);		//Первый байт будильника (статус и 7 бит дней недели)
		if(CheckBit(atmp,0)){lcd_str("ON   ");}else{lcd_str("OFF  ");}// статус будильника ON, OFF
		lcd_goto_xy(1,4);
		if(mode==0){lcd_str("              ");}
		if(mode==1){lcd_str("^            ");}
		if(mode==2){lcd_str("  ^          ");}
		if(mode==3){lcd_str("     ^       ");}
		if(mode==4){lcd_str("        ^    ");}
		if(mode==5){lcd_str("V            ");}
		if(mode==6){lcd_str("  V          ");}
		if(mode==7){lcd_str("    V        ");}
		if(mode==8){lcd_str("      V      ");}
		if(mode==9){lcd_str("        V    ");}
		if(mode==10){lcd_str("          V  ");}
		if(mode==11){lcd_str("            V");}
		lcd_str("               ");
		if(CheckBit(atmp,1)){lcd_chrWD(1,2,5);}
		if(CheckBit(atmp,2)){lcd_chrWD(2,14,5);}
		if(CheckBit(atmp,3)){lcd_chrWD(3,25,5);}
		if(CheckBit(atmp,4)){lcd_chrWD(4,36,5);}
		if(CheckBit(atmp,5)){lcd_chrWD(5,48,5);}
		if(CheckBit(atmp,6)){lcd_chrWD(6,60,5);}
		if(CheckBit(atmp,7)){lcd_chrWD(7,73,5);}
	}
	if(menu==4){lcd_clear();delay(1000);menu=0;}	//exit at main menu
}
//***************************************************************************
int main (void){
	unsigned char bm,bs,i;
	sei();
	avr_init();
	lcd_init();
	t0_init();
	while(1){
		bm=bs=0;
		if(CheckBit(PIN_BUTTON,BMODE)==0){	// Нажата кнопка MODE
			// Программный антидребезг контактов
			for(i=0; i < 250; i++){delay(200);if(CheckBit(PIN_BUTTON,BMODE)==0){bm++;bgl=0;}}
		}
		if(CheckBit(PIN_BUTTON,BSET)==0){	// Нажата кнопка SET
			// Программный антидребезг контактов
			for(i=0; i < 250; i++){delay(200);if(CheckBit(PIN_BUTTON,BSET)==0){bs++;bgl=0;}}
		}
		if(bm>140 && !menu){bm=0;drawMenu();}
		if(bm>70 && mode==0 && menu>0 && menu<4){menu++;drawMenu();}
		if(bs>70 && menu==1 && mode==0){bs=0;mode=1;drawMenu();}
		if(bs>70 && menu==2 && mode==0){bs=0;mode=1;drawMenu();}
		if(bs>70 && menu==3 && mode==0){bs=0;mode=1;drawMenu();}
		if(bm>70 && menu==1 && mode>0){if(mode<3){mode++;}else{mode=0;}drawMenu();}
		if(bs>70 && menu==1 && mode==1){Hour++;}
		if(bs>70 && menu==1 && mode==2){if(bs>240){Min+=5;}else{Min++;}}
		if(bs>70 && menu==1 && mode==3){Sec=0;}
		if(bs>70 && menu==1 && mode>0 && mode<4){
			if(Hour>23){Hour=0;}
			if(Min>59){Min=0;}
			ds1302_SetNewTime(Sec,Min,Hour);
			drawMenu();
		}
		if(bm>70 && menu==2 && mode>0){if(mode<4){mode++;}else{mode=0;}drawMenu();}
		if(bs>70 && menu==2 && mode==1){if(bs>240){Day+=5;}else{Day++;}}
		if(bs>70 && menu==2 && mode==2){Month++;}
		if(bs>70 && menu==2 && mode==3){if(bs>240){Year+=5;}else{Year++;}}
		if(bs>70 && menu==2 && mode==4){DayOfWeek++;}
		if(bs>70 && menu==2 && mode>0 && mode<5){
			if(Day>31){Day=1;}
			if(Month>12){Month=1;}
			if(Year>99){Year=0;}
			if(DayOfWeek>7){DayOfWeek=1;}
			ds1302_SetNewDate(Day,Month,Year,DayOfWeek);
			drawMenu();
		}
		if(bm>70 && menu==3 && mode>0){if(mode<11){mode++;}else{mode=0;}drawMenu();}
		if(bs>70 && menu==3 && mode==1){bs=0;if(alarmNum<9){alarmNum+=3;}else{alarmNum=0;}drawMenu();}
		if(bs>70 && menu==3 && mode==2){
			bs=0;
			i=eeprom_read(alarmNum+1);
			if(i<23){i++;}else{i=0;}
			eeprom_write(alarmNum+1,i);
			drawMenu();
		}
		if(bs>70 && menu==3 && mode==3){
			bs=0;
			i=eeprom_read(alarmNum+2);
			if(i<59){i++;}else{i=0;}
			eeprom_write(alarmNum+2,i);
			drawMenu();
		}
		if(bs>70 && menu==3 && mode>=4){
			bs=0;
			i=eeprom_read(alarmNum);
			ibi(i,mode-4);
			eeprom_write(alarmNum,i);
			drawMenu();
		}
		delay(1000);
	}
}