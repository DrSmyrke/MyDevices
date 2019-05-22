#define F_CPU 8000000UL

#define MODE_EEPROM_ADDR			0x0A
#define BRIGHTNESS_EEPROM_ADDR		0x0A

#define LED_MODE1_PORT		PORTB
#define LED_MODE1_PIN		1
#define LED_MODE1_INIT		pinModePB(LED_MODE1_PIN,OUTPUT)
#define LED_MODE1_ON		digitalWrite(LED_MODE1_PORT,LED_MODE1_PIN,0)
#define LED_MODE1_OFF		digitalWrite(LED_MODE1_PORT,LED_MODE1_PIN,1)
#define LED_MODE2_PORT		PORTB
#define LED_MODE2_PIN		2
#define LED_MODE2_INIT		pinModePB(LED_MODE2_PIN,OUTPUT)
#define LED_MODE2_ON		digitalWrite(LED_MODE2_PORT,LED_MODE2_PIN,0)
#define LED_MODE2_OFF		digitalWrite(LED_MODE2_PORT,LED_MODE2_PIN,1)
#define LED_MODE3_PORT		PORTB
#define LED_MODE3_PIN		3
#define LED_MODE3_INIT		pinModePB(LED_MODE3_PIN,OUTPUT)
#define LED_MODE3_ON		digitalWrite(LED_MODE3_PORT,LED_MODE3_PIN,0)
#define LED_MODE3_OFF		digitalWrite(LED_MODE3_PORT,LED_MODE3_PIN,1)
#define LED_MODE4_PORT		PORTB
#define LED_MODE4_PIN		4
#define LED_MODE4_INIT		pinModePB(LED_MODE4_PIN,OUTPUT)
#define LED_MODE4_ON		digitalWrite(LED_MODE4_PORT,LED_MODE4_PIN,0)
#define LED_MODE4_OFF		digitalWrite(LED_MODE4_PORT,LED_MODE4_PIN,1)
#define LED_POWER_PORT		PORTB
#define LED_POWER_PIN		5
#define LED_POWER_INIT		pinModePB(LED_POWER_PIN,OUTPUT)
#define LED_POWER_ON		digitalWrite(LED_POWER_PORT,LED_POWER_PIN,0)
#define LED_POWER_OFF		digitalWrite(LED_POWER_PORT,LED_POWER_PIN,1)

#define MODE_BUTTON_INIT	pinModePD(2,INPUT)
#define SET_BUTTON_INIT		pinModePD(3,INPUT)
#define MODE_BUTTON_READ	CheckBit(PIND,2)
#define SET_BUTTON_READ		CheckBit(PIND,3)

#define ODOMETR_MULTIPLIER	100

#include "main.h"
#include "uart.h"
#include "T1637.h"
#include "Atmega8T0.h"
#include "comparator.h"
#include "eeprom.h"
//#include "INT0.h"
//#include "INT1.h"
//***************************************************************************
// Установки
uint8_t viewValue = 0;
uint8_t prewSpeed = 0;
uint8_t modeButton = false;
uint8_t setButton = false;
uint8_t brightness = 7;
uint8_t mode = 1;
uint8_t modeCounter = 3;
//***************************************************************************
// Инициализация
void avr_init(void)
{
	gps.odometr = 0;
	LED_MODE1_INIT;
	LED_MODE2_INIT;
	LED_MODE3_INIT;
	LED_MODE4_INIT;
	LED_POWER_INIT;
	LED_MODE1_OFF;
	LED_MODE2_OFF;
	LED_MODE3_OFF;
	LED_MODE4_OFF;
	LED_POWER_OFF;
	delay(10000);
	LED_MODE1_ON;delay2();
	LED_MODE2_ON;delay2();
	LED_MODE3_ON;delay2();
	LED_MODE4_ON;delay2();
	LED_POWER_ON;delay2();
	delay(10000);
	LED_MODE1_OFF;
	LED_MODE2_OFF;
	LED_MODE3_OFF;
	LED_MODE4_OFF;
	LED_POWER_OFF;

	MODE_BUTTON_INIT;
	SET_BUTTON_INIT;
}
//***************************************************************************
//Функция вызыается 10 раз в секунду или каждые 100мс
void ms100(void)
{
	static uint8_t DispData[] = { 0x10, 0x10, 0x10, 0x10 };
	
	// монитор питания
	digitalWrite(LED_POWER_PORT,LED_POWER_PIN,!CheckBit(ACSR,ACO));

	if( modeCounter ){
		modeCounter--;
//		if( modeCounter == 0 ){
//			if( mode++ > 4 ) mode = 1;
//			eeprom_write( MODE_EEPROM_ADDR, mode );
//		}
	}

	
	if( mode > 4 ) mode = 1;
	DispData[1] = mode;
	switch (mode){
		case 1:
			LED_MODE1_ON;
			LED_MODE2_OFF;
			LED_MODE3_OFF;
			LED_MODE4_OFF;
			if( modeCounter ){
				TM1637_display_all(DispData);
			}else{
				TM1637_display_int_decimal(gps.sputniks,true);
			}
		break;
		case 2:
			LED_MODE1_OFF;
			LED_MODE2_ON;
			LED_MODE3_OFF;
			LED_MODE4_OFF;
			if( modeCounter ){
				TM1637_display_all(DispData);
			}else{
				TM1637_display_int_decimal(gps.odometr,true);
			}
		break;
		case 3:
			LED_MODE1_OFF;
			LED_MODE2_OFF;
			LED_MODE3_ON;
			LED_MODE4_OFF;
			if( modeCounter ){
				TM1637_display_all(DispData);
			}else{
				if( viewValue ){
					TM1637_display_int_decimal(gps.speed,true);
				}else{
					DispData[1] = 0x10;
					TM1637_display_all(DispData);
				}
			}
		break;
		case 4:
			LED_MODE1_OFF;
			LED_MODE2_OFF;
			LED_MODE3_OFF;
			LED_MODE4_ON;
			if( modeCounter ){
				TM1637_display_all(DispData);
			}else{
				TM1637_display_int_decimal(brightness,true);
			}
		break;
	}
}
//Функция вызыается раз в секунду
void second(void)
{
	static uint8_t secCounter = 0;

	//Каждые 30 секунд сохраняем пройденое расстояние в EEPROM
/*
	if( secCounter >= 30){
		eeprom_write( 1, gps.odometr1 );
		eeprom_write( 2, gps.odometr2 );
		eeprom_write( 3, gps.odometr3 );
		eeprom_write( 4, gps.odometr4 );
		eeprom_write( 5, gps.odometr5 );
		secCounter = 0;
	}else{
		secCounter ++;
	}
*/
	// проверка
	if( gps.speedValid != SPEED_VALID || gps.sputniks <= 5 || !gps.myPosFound ){
		viewValue = false;
		prewSpeed = 0;
	}else{
		viewValue = true;
	}	

	if( prewSpeed == 0 ){
		prewSpeed = gps.speed;
		return;
	}
	//расчет расстояния в км за час на основе предыдущей скорости
	uint8_t r = ( prewSpeed + gps.speed ) / 2;				//средняя скорость за прошедшую секнду
	gps.odometr1 += r;
	if( gps.odometr1 > ODOMETR_MULTIPLIER ){
		gps.odometr1 -= ODOMETR_MULTIPLIER;
		gps.odometr2 ++;
	}
	if( gps.odometr2 > ODOMETR_MULTIPLIER ){
		gps.odometr2 -= ODOMETR_MULTIPLIER;
		gps.odometr3 ++;
	}
	if( gps.odometr3 > ODOMETR_MULTIPLIER ){
		gps.odometr3 -= ODOMETR_MULTIPLIER;
		gps.odometr4 ++;
	}
	if( gps.odometr4 > ODOMETR_MULTIPLIER ){
		gps.odometr4 -= ODOMETR_MULTIPLIER;
		gps.odometr5 ++;
	}
	gps.odometr = (
				( gps.odometr5 * ODOMETR_MULTIPLIER ) + 
				( gps.odometr4 * ODOMETR_MULTIPLIER ) + 
				( gps.odometr3 * ODOMETR_MULTIPLIER ) + 
				( gps.odometr2 * ODOMETR_MULTIPLIER ) + 
				gps.odometr1 ) / 3600;
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
	
	gps.odometr1 = eeprom_read( 1 );
	gps.odometr2 = eeprom_read( 2 );
	gps.odometr3 = eeprom_read( 3 );
	gps.odometr4 = eeprom_read( 4 );
	gps.odometr5 = eeprom_read( 5 );
	brightness = eeprom_read( BRIGHTNESS_EEPROM_ADDR );
	mode = eeprom_read( MODE_EEPROM_ADDR );

	TM1637_init(D4036B);				//D4036B
	TM1637_setBrightness(brightness);
	_PointFlag = 1;
	
  	while(1){
		uint8_t i;
		modeButton = false;
		setButton = false;

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
			TM1637_setBrightness(brightness);
			eeprom_write( BRIGHTNESS_EEPROM_ADDR, brightness );
		}
  	}
}
