#include "main.h"
#include "functions.h"

int main(void)
{
	Mouse.begin();
	Keyboard.begin();
	Serial.begin( 9600 );
	pinMode( LED_BUILTIN, OUTPUT );

	fan_on();
	for( ;; ){
		digitalWrite( LED_BUILTIN, !digitalRead( LED_BUILTIN ) );
		delay( 500 );
	}
}
