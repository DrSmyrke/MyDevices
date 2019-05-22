#define F_CPU 8000000UL

#define MODE_EEPROM_ADDR			0x0A

#define LED_POWER_PORT		PORTB
#define LED_POWER_PIN		0
#define LED_POWER_INIT		pinModePB(LED_POWER_PIN,OUTPUT)
#define LED_POWER_ON		digitalWrite(LED_POWER_PORT,LED_POWER_PIN,0)
#define LED_POWER_OFF		digitalWrite(LED_POWER_PORT,LED_POWER_PIN,1)

#define MODE_BUTTON_INIT	pinModePD(2,INPUT)
#define SET_BUTTON_INIT		pinModePD(3,INPUT)
#define MODE_BUTTON_READ	CheckBit(PIND,2)
#define SET_BUTTON_READ		CheckBit(PIND,3)

#include "main.h"
#include "lcd5110.h"
#include "nmea.h"
#include "uart.h"
#include "Atmega8T0.h"
#include "comparator.h"
#include "eeprom.h"
#include "lcd5110.c"
//#include "INT0.h"
//#include "INT1.h"
//***************************************************************************
// Установки
uint8_t viewValue = 0;
uint8_t modeButton = false;
uint8_t setButton = false;
uint8_t brightness = 7;
uint8_t mode = 1;
uint8_t modeCounter = 3;
uint8_t validDataCounter = 0;
//***************************************************************************
// Инициализация
void avr_init(void)
{
	lcd_init();
	lcd_contrast(65);
	lcd_goto_xy(1,2);lcd_str("  Loading...  ");

	gps.odometr = 0;
	gps.speed = 0;
	gps.speedDrob = 0;
	gps.altitude = 0;
	gps.altitudeDrob = 0;
	gps.sputniks = 0;

	LED_POWER_INIT;
	LED_POWER_OFF;
	delay(10000);
	LED_POWER_ON;delay2();
	delay(10000);
	LED_POWER_OFF;

	MODE_BUTTON_INIT;
	SET_BUTTON_INIT;
}
//***************************************************************************
//Функция вызыается 10 раз в секунду или каждые 100мс
void ms100(void)
{
	// монитор питания
	digitalWrite(LED_POWER_PORT,LED_POWER_PIN,!CheckBit(ACSR,ACO));

//	if( modeCounter ){
//		modeCounter--;
//		if( modeCounter == 0 ){
//			if( mode++ > 4 ) mode = 1;
//			eeprom_write( MODE_EEPROM_ADDR, mode );
//		}
//	}
}
//Функция вызыается раз в секунду
void second(void)
{
	static uint8_t prewSpeed = 0;
	static uint8_t prewSpeedDrob = 0;
	// проверка
	if( gps.speedValid != SPEED_VALID || gps.sputniks <= 5 || !gps.myPosFound ){
		prewSpeed = 0;
		validDataCounter = 0;
		return;
	}

	//Если в течении 3-х секунд данные коректны работаем со скоростью
	if( validDataCounter < 3 ){
		validDataCounter++;
		return;
	}

	//Ходьба не учитывается
	if( gps.speed < 6 ){
		prewSpeed = 0;
		return;
	}

	if( prewSpeed == 0 ){
		prewSpeed = gps.speed;
		prewSpeedDrob = gps.speedDrob;
		return;
	}
	//расчет расстояния в км за час на основе предыдущей скорости
	gps.odometr += ( prewSpeed + gps.speed ) / 2; //средняя скорость за прошедшую секнду
	gps.odometrDrob += ( ( prewSpeed + gps.speed ) / 2 ) % 10;

	gps.odometrDrob += ( prewSpeedDrob + gps.speedDrob ) / 2;
	while( gps.odometrDrob >= 10 ){
		gps.odometr++;
		gps.odometrDrob -= 10;
	}
	

	prewSpeed = gps.speed;
	prewSpeedDrob = gps.speedDrob;
}
//***************************************************************************
int main (void)
{
	avr_init();
 	uart_init();
	t0_init();
 	comparator_init();
	//int0_init();
	//int1_init();uint8_t i = 0;
 	sei();
	
	//gps.odometr1 = eeprom_read( 1 );
	//gps.odometr2 = eeprom_read( 2 );
	//gps.odometr3 = eeprom_read( 3 );
	//gps.odometr4 = eeprom_read( 4 );
	//gps.odometr5 = eeprom_read( 5 );
	mode = eeprom_read( MODE_EEPROM_ADDR );

	lcd_clear();

	uint8_t counter = 0;
	uint16_t odoRes = 0;
	uint8_t odoResDrob = 0;
	
  	while(1){
		//uint8_t i;
		modeButton = false;
		setButton = false;

		nmea_dataCheck();

		

		switch( counter ){
			case 1:
				lcd_goto_xy(2,1);
				lcd5110_print_uchar_decimal( gps.sputniks, false, false );
				lcd_goto_xy(5,1); if( gps.myPosFound > 0 ){ lcd_chr( '+' ); }else{ lcd_chr( '-' ); }
				lcd_goto_xy(6,1); if( gps.speedValid == SPEED_VALID ){ lcd_chr( '+' ); }else{ lcd_chr( '-' ); }
				lcd_goto_xy(7,1); lcd5110_print_short_decimal( gps.altitude, true, true );lcd_chr( '.' );lcd_chr( 0x30 + gps.altitudeDrob );
				//DEBUG
				lcd_goto_xy(1,2); lcd_str( gpsRaw.speed );lcd_chr( '|' );lcd_chr( gpsRaw.sputniks[0] );lcd_chr( gpsRaw.sputniks[1] );
			break;
			case 2:
				//lcd_chrNum( gps.speed / 100, 4, 3 );
				lcd_chrNum( (gps.speed / 10) % 10, 22, 3 );
				lcd_chrNum( gps.speed % 10 , 40, 3 );
				lcd_goto_xy(10,4);lcd_str(".");
				lcd_chrNum( gps.speedDrob , 60, 3 );
			break;
			//case 3:
				//odoRes = gps.odometr / 3600;
			//break;
			//case 4:
				//odoResDrob = (gps.odometr / 3600) % 10;
			//break;
			//case 5:
				//lcd_goto_xy(7,6);lcd5110_print_short_decimal( gps.odometr, true, true );lcd_chr( '.' );lcd_chr( 0x30 + odoResDrob );
			//	lcd_goto_xy(7,5);lcd5110_print_short_decimal( odoRes, true, true );lcd_chr( '.' );lcd_chr( 0x30 + odoResDrob );
			//break;
			case 3:
				lcd_goto_xy(7,6);lcd5110_print_ushort_decimal( gps.odometr, true, true );lcd_chr( '.' );lcd_chr( 0x30 + gps.odometrDrob );
			break;
		}

		

		

		
/*
		if( MODE_BUTTON_READ && !modeCounter ){
			for( i = 0; i < 100; i++ ) delay(500);
			if( MODE_BUTTON_READ ) modeButton = true;
		}
		if( SET_BUTTON_READ && !modeCounter ){
			for( i = 0; i < 100; i++ ) delay(500);
			if( SET_BUTTON_READ ) setButton = true;
		}

		if( modeButton ){
			if( mode++ > 4 ) mode = 1;
			modeCounter = 3;
			eeprom_write( MODE_EEPROM_ADDR, mode );
		}
		if( setButton && mode == 4 ){
			if( brightness++ > 7 ) brightness = 0;
			//TM1637_setBrightness(brightness);
			//eeprom_write( BRIGHTNESS_EEPROM_ADDR, brightness );
		}
*/
		delay4();
		if( counter++ > 3 ) counter = 1;
  	}
}
