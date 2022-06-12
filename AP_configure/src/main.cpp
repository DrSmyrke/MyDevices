#include "main.h"
#include "functions.h"
#include "timer.h"
#include "http.h"

//-------------------------------------------------------------------------------
ESP8266WebServer webServer( 80 );
MainFlags flags;
const IPAddress apIP( 10, 10, 10, 10 );
const IPAddress apMASK( 255, 255, 255, 0 );



//-------------------------------------------------------------------------------
void setup()
{
	flags.timer									= 0;
	flags.access								= 0;

	LittleFS.begin();
	Serial.begin( 9600 );
	pinMode( LED_BUILTIN, OUTPUT );

	delay( 100 );

	wifi_AP_init();
	timer_init();

	webServer.on( "/", handleRoot );
	webServer.on( "/wifi", handleWifiConfig );
	webServer.onNotFound( handleNotFound );
	webServer.begin();

	delay( 100 );
}

//-------------------------------------------------------------------------------
void loop()
{
	if( flags.timer ){
		flags.timer = 0;

		digitalWrite( LED_BUILTIN, !flags.access );
	}

	
	webServer.handleClient();
}

