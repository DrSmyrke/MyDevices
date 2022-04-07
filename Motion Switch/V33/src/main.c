//--- INCLUDES ------------------------------------------------------------------------------
#include "main.h"
#include <stdio.h>
#include <support_defines.h>

//--- DEFINES -------------------------------------------------------------------------------
#define LED_PORT				PORTD
#define LED_PIN					4
#define LED_INIT				pinModePD(LED_PIN,OUTPUT)
#define LEDS_COUNT				11
#define DECRIMENT				100

#define SLEEP_SEC_INIT			15

//--- VARIABLES -----------------------------------------------------------------------------
#ifdef __UART
	char str[ 64 ];
	char tempVal[ 10 ];
#endif
struct cRGB led[ LEDS_COUNT ];

uint8_t sleepSec = SLEEP_SEC_INIT;

//--- FUNCTIONS -----------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
uint8_t setBright( uint8_t x, uint8_t b )
{
	return (((uint16_t)(x) * ((b) + 1)) >> 8);
}

//-------------------------------------------------------------------------------------------
void led_clear(void)
{
	uint8_t i;
	for( i = 0; i < LEDS_COUNT; i++){    
		led[ i ].r = 0; led[ i ].g = 0; led[ i ].b = 0;
	}
	ws2812_sendarray( (uint8_t *)led, LEDS_COUNT * 3 );
}

//-------------------------------------------------------------------------------------------
void init(void)
{
	LED_INIT;
	DDRD |=_BV(ws2812_pin);
	led_clear();
	int0_init();
#ifdef __UART
	uart_init();
#endif
	sleep_init();
	sleep_enable();
	t0_init();
	adc_init();
	sei();
}

//-------------------------------------------------------------------------------------------
void setImpulse(void)
{
	led[ 1 ] = led[ 0 ];
}

//-----------------------------------------------------------------------
void int0_interrupt(void)
{
	sleepSec += 10;
}

//-------------------------------------------------------------------------------------------
int main(void)
{
	init();

	pinModePD( 7, OUTPUT );

	while( 1 ){

		if( ms100Flag ){
			ms100Flag = 0;
			
			ws2812_sendarray( (uint8_t *)led, LEDS_COUNT * 3 );

			uint8_t i;
			for( i = LEDS_COUNT - 1; i > 1; i-- ){    
				led[ i ] = led[ i - 1 ];
			}

			if( led[ 1 ].r != 0 || led[ 1 ].g != 1 || led[ 1 ].b != 0 ){

				// if( led[ 1 ].r >= DECRIMENT ){
					led[ 1 ].r = setBright( led[ 2 ].r, DECRIMENT );
				// }else{
				// 	led[ 0 ].r = 0;
				// }
				// if( led[ 1 ].g >= DECRIMENT ){
					led[ 1 ].g = setBright( led[ 2 ].g, DECRIMENT );
				// }else{
				// 	led[ 0 ].g = 0;
				// }
				// if( led[ 1 ].b >= DECRIMENT ){
					led[ 1 ].b = setBright( led[ 2 ].b, DECRIMENT );
				// }else{
				// 	led[ 0 ].b = 0;
				// }
				// mData data = getFade( mRGB( led[ 0 ].r, led[ 0 ].g, led[ 0 ].b ), 25 );
				
				// led[ 0 ].r = 0; led[ 0 ].g = 0; led[ 0 ].b = 0;
				// led[ 0 ].r = 0; led[ 0 ].g = 0; led[ 0 ].b = 0;
			}





#ifdef __UART
			str_uart_send( str );
#endif
			adc_run();


			if( vcc5 < 50 ){
				//violet
				// led[ 0 ].r = 100; led[ 0 ].g = 000; led[ 0 ].b = 255;
				// led[ 0 ].r = 100; led[ 0 ].g = 000; led[ 0 ].b = 100;
				led[ 0 ].r = setBright( 50, DECRIMENT );
				led[ 0 ].g = 0;
				led[ 0 ].b = setBright( 50, DECRIMENT );
			}else if( vcc5 < 150 ){
				//blue
				// led[ 0 ].r = 000; led[ 0 ].g = 000; led[ 0 ].b = 255;	
				// led[ 0 ].r = 000; led[ 0 ].g = 000; led[ 0 ].b = 100;
				led[ 0 ].r = 000; led[ 0 ].g = 000; led[ 0 ].b = 50;
			}else if( vcc5 < 300 ){
				//light blue
				// led[ 0 ].r = 000; led[ 0 ].g = 100; led[ 0 ].b = 255;	
				// led[ 0 ].r = 000; led[ 0 ].g = 100; led[ 0 ].b = 100;
				led[ 0 ].r = 000; led[ 0 ].g = 100; led[ 0 ].b = 100;

			// 	led[ 0 ].r = 255; led[ 0 ].g = 000; led[ 0 ].b = 000;	//red
			}else if( vcc5 < 550 ){
				led[ 0 ].r = 000; led[ 0 ].g = 50; led[ 0 ].b = 000;	//green
			// }else if( vcc5 < 600 ){
			// 	led[ 0 ].r = 255; led[ 0 ].g = 100; led[ 0 ].b = 000;	//orange
			// }else if( vcc5 < 700 ){
			// 	led[ 0 ].r = 100; led[ 0 ].g = 255; led[ 0 ].b = 000;	//yellow
			// }else if( vcc5 < 1000 ){
			// 	led[ 0 ].r = 200; led[ 0 ].g = 200; led[ 0 ].b = 200;	//white
			}else{
				led[ 0 ].r = 0; led[ 0 ].g = 0; led[ 0 ].b = 0;
				sleepSec = 0;
			}
		}

		if( secondFlag ){
			secondFlag = 0;

			digitalWrite( PORTD, 7, !CheckBit( PIND, 7 ) );

			setImpulse();

			if( sleepSec ) sleepSec--;
		}

		if( !sleepSec ){
			digitalWrite( PORTD, 7, 0 );
			led_clear();
			sleep();
			sleepSec = 255;
			while( sleepSec-- ) asm( "nop" );
			sleepSec = 15;
			t0_reset();
		}

#ifdef __UART
		strcpy( str, "$" ); itoa( vcc5, tempVal, 10 ); strcat( str, tempVal );
		strcat( str, " " ); itoa( vcc3, tempVal, 10 ); strcat( str, tempVal );
		strcat( str, ";" );
#endif
	}
}
