//--- DEFINES -------------------------------------------------------------------------------
#define LED_PORT				PORTD
#define LED_PIN					4
#define LED_INIT				pinModePD(LED_PIN,OUTPUT)
#define LEDS_COUNT				11

#define SLEEP_SEC_INIT			3

//--- INCLUDES ------------------------------------------------------------------------------
#include "main.h"
#include "Atmega8T0.h"
#include "sleep.h"
#include "INT0.h"
#include "adc.h"
#include "uart.h"
#include "watchhog.h"
// #include "color.h"
// #include "color_utility.h"

#include <stdlib.h>
#include <light_ws2812.h>
#include <Arduino.h>

analogRead( A2 );

//--- VARIABLES -----------------------------------------------------------------------------
uint8_t sleepSec = SLEEP_SEC_INIT;



#define COLORLENGTH (LEDS_COUNT/2)
#define FADE (256/COLORLENGTH)
#define DECRIMENT			100


struct cRGB led[LEDS_COUNT];
char str[ 32 ];
char tempVal[ 10 ];



//-----------------------------------------------------------------------
void led_clear(void)
{
	uint8_t i;
	for( i = 0; i < LEDS_COUNT; i++){    
		led[ i ].r = 0; led[ i ].g = 0; led[ i ].b = 0;
	}
	ws2812_sendarray( (uint8_t *)led, LEDS_COUNT * 3 );
}

//-----------------------------------------------------------------------
void init(void)
{
	LED_INIT;
	DDRD |=_BV(ws2812_pin);
	led_clear();
}

//-----------------------------------------------------------------------
void int0_interrupt(void)
{
	sleepSec += 10;
}

//Функция вызыается раз в секунду
void second(void)
{
	if( sleepSec ) sleepSec--;

	led[ 0 ].r = 255; led[ 0 ].g = 100; led[ 0 ].b = 000;

	if( vcc5 < 250 ){
		led[ 0 ].r = 100; led[ 0 ].g = 000; led[ 0 ].b = 255;	//violet
	}else if( vcc5 < 500 ){
		led[ 0 ].r = 000; led[ 0 ].g = 000; led[ 0 ].b = 255;	//blue
	}else if( vcc5 < 1024 ){
		led[ 0 ].r = 000; led[ 0 ].g = 100; led[ 0 ].b = 255;	//light blue
	// }else if( vcc5 < 500 ){
	// 	led[ 0 ].r = 255; led[ 0 ].g = 000; led[ 0 ].b = 000;	//red
	// }else if( vcc5 < 550 ){
	// 	led[ 0 ].r = 000; led[ 0 ].g = 255; led[ 0 ].b = 000;	//green
	// }else if( vcc5 < 600 ){
	// 	led[ 0 ].r = 255; led[ 0 ].g = 100; led[ 0 ].b = 000;	//orange
	// }else if( vcc5 < 700 ){
	// 	led[ 0 ].r = 100; led[ 0 ].g = 255; led[ 0 ].b = 000;	//yellow
	// }else if( vcc5 < 1000 ){
	// 	led[ 0 ].r = 200; led[ 0 ].g = 200; led[ 0 ].b = 200;	//white
	}

	uart_send( '1' );
}

uint8_t setBright( uint8_t x, uint8_t b )
{
	return (((uint16_t)(x) * ((b) + 1)) >> 8);
}

int main(void)
{
	init();
	t0_init();
	int0_init();
	sleep_init();
	adc_init();
	sleep_enable();
	uart_init();
	wdt_init();
	sei();

	while( 1 ){

		if( ms100Flag ){
			ms100Flag = 0;

			ws2812_sendarray( (uint8_t *)led, LEDS_COUNT * 3 );

			uint8_t i;
			for( i = LEDS_COUNT - 1; i > 0; i-- ){    
				led[ i ] = led[ i - 1 ];
			}


			// if( led[ 0 ].r >= DECRIMENT ){ led[ 0 ].r -= DECRIMENT; }else{ led[ 0 ].r = 0; }
			// if( led[ 0 ].g >= DECRIMENT ){ led[ 0 ].g -= DECRIMENT; }else{ led[ 0 ].g = 0; }
			// if( led[ 0 ].b >= DECRIMENT ){ led[ 0 ].b -= DECRIMENT; }else{ led[ 0 ].b = 0; }

			
			if( led[ 0 ].r != 0 || led[ 0 ].g != 1 || led[ 0 ].b != 0 ){

				// if( led[ 1 ].r >= DECRIMENT ){
					led[ 0 ].r = setBright( led[ 1 ].r, DECRIMENT );
				// }else{
				// 	led[ 0 ].r = 0;
				// }
				// if( led[ 1 ].g >= DECRIMENT ){
					led[ 0 ].g = setBright( led[ 1 ].g, DECRIMENT );
				// }else{
				// 	led[ 0 ].g = 0;
				// }
				// if( led[ 1 ].b >= DECRIMENT ){
					led[ 0 ].b = setBright( led[ 1 ].b, DECRIMENT );
				// }else{
				// 	led[ 0 ].b = 0;
				// }
				// mData data = getFade( mRGB( led[ 0 ].r, led[ 0 ].g, led[ 0 ].b ), 25 );
				
				// led[ 0 ].r = 0; led[ 0 ].g = 0; led[ 0 ].b = 0;
				// led[ 0 ].r = 0; led[ 0 ].g = 0; led[ 0 ].b = 0;
			}

			// adc_run();

			// strcpy( str, "$" ); itoa( vcc5, tempVal, 10 ); strcat( str, tempVal );
			// strcat( str, " " ); itoa( vcc3, tempVal, 10 ); strcat( str, tempVal );
			// strcat( str, ";" );
			// str_uart_send( str );
		}
	
		if( !adc_isLight() ){
		// 	LED1_ON;
		}else{
			sleepSec = 0;
		}
		
		if( !sleepSec ){
			led_clear();
			t0_reset();
			sleep();
			sleepSec += 75;
		}


		//shift all vallues by one led
		

		//for( i = LEDS_COUNT; i > 1; i-- ) led[ i - 1 ] = led[ i - 2 ];
		//change colour when colourlength is reached   
		// if( k > COLORLENGTH ){
		// 	j++;
		// 	if( j > 7 ){ j = 0; }
		// 	k = 0;
		// }
		// k++;
		//loop colouers
		
		//fade red
		// if( led[0].r<(colors[j].r-FADE) ) led[0].r+=FADE;
		// if( led[0].r>(colors[j].r+FADE) ) led[0].r-=FADE;
		// if( led[0].g<(colors[j].g-FADE) ) led[0].g+=FADE;
		// if( led[0].g>(colors[j].g+FADE) ) led[0].g-=FADE;
		// if( led[0].b<(colors[j].b-FADE) ) led[0].b+=FADE;
		// if( led[0].b>(colors[j].b+FADE) ) led[0].b-=FADE;

		// _delay_ms( 10 );
		
	}
}
