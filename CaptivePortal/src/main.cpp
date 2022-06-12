#include "main.h"
#include "functions.h"
#include "timer.h"
#include "http.h"

//-------------------------------------------------------------------------------
DNSServer dnsServer;
ESP8266WebServer webServer( 80 );
MainFlags flags;
const IPAddress apIP( 10, 10, 10, 10 );
const IPAddress apMASK( 255, 255, 255, 0 );
int8_t countNetworks;




//-------------------------------------------------------------------------------
void setup()
{
	flags.apMode								= 1;
	flags.timer									= 0;
	flags.captivePortalAccess					= 0;
	flags.access								= 0;

	Serial.begin( 9600 );
	pinMode( LED_BUILTIN, OUTPUT );

	delay( 100 );
	countNetworks = WiFi.scanNetworks();

	wifi_AP_init();

	timer_init();

	webServer.on( "/", handleRoot );
	//Captive portal
	if( flags.apMode ){
		webServer.on( "/fwlink", handleCaptivePortal );
		webServer.on( "/generate_204", handleCaptivePortal );
		webServer.on( "/favicon.ico", handleCaptivePortal );
	}
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
	
	if( flags.apMode ){
		dnsServer.processNextRequest();
	}
}

