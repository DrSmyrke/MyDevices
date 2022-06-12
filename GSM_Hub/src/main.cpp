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
ATCommand AT;
GSM_Data gsmData;
char tmpVal[ 10 ];
char gsm_data_buff[ 512 ];
long wtimer										= 0;
uint8_t check_counter							= 0;


//-------------------------------------------------------------------------------
void setup()
{
	flags.apMode								= 0;
	flags.timer									= 0;
	flags.captivePortalAccess					= 0;
	countNetworks								= 0;

	LittleFS.begin();
	Serial.begin( 115200 );
	Serial.swap();
	delay( 200 );
	Serial.flush();

	pinMode( LED_BUILTIN, OUTPUT );
	digitalWrite( LED_BUILTIN, HIGH );
	pinMode( GSM_RESET_PIN, OUTPUT );
	// GSM_BEGIN;
	GSM_RESET();
	delay( 3000 );
	sendATCommand( "AT", true );
	sendATCommand( GSM_SMS_INIT, true );



	delay( 100 );
	countNetworks = WiFi.scanNetworks();

	if( esp::isClient() ){
		wifi_STA_init();
	}else{
		flags.apMode							= 1;
		wifi_AP_init();
	}

	timer_init();

	webServer.on( "/", handleRoot );
	webServer.on( "/info", handleInfo );
	webServer.on( "/exec", handleExec );
	webServer.on( "/wifi", handleWifiConfig );
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

		if( !flags.apMode ){
			if( !WiFiConnectionState() ){
				digitalWrite( LED_BUILTIN, HIGH );
				WiFi.reconnect();
			}else{
				digitalWrite( LED_BUILTIN, LOW );
			}
			// ArduinoOTA.handle();
		}

		switch( check_counter++ ){
			case 10:	sendATCommand( "AT+CREG?", false );	break;
			case 20:	sendATCommand( "AT+COPS?", false );	break;
			case 30:	sendATCommand( "AT+CSQ?", false );	break;
			case 40:	check_counter = 0;	break;
			default: break;
		}
	}

	if( GSM_AVAILABLE ){
		char sym = (char) GSM_READ;
		AT.addSymbol( sym );
#ifdef __DEV
		Serial.print( sym );
#endif
		// Serial.print( ":" );Serial.println( AT.getData() );
		
		GSM_responseProcess();
	}

	webServer.handleClient();
	
	if( flags.apMode ){
		dnsServer.processNextRequest();
	}
}

