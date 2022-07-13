#include "main.h"
#include "functions.h"
#include <timer.h>
#include "http.h"

//-------------------------------------------------------------------------------
SoftwareSerial GSM_PORT( D5, D6 );
Timer timer0( 0, 1000 );
DNS_Server dnsServer;
ESP8266WebServer webServer( 80 );
AT at( &GSM_PORT, GSM_responseProcess );
GSM_Data gsmData;
char tmpVal[ 10 ];
char pageBuff[ WEB_PAGE_BUFF_SIZE ];
char gsm_data_buff[ GSM_DATA_BUFF_SIZE ];
uint8_t check_counter					= 0;

//-------------------------------------------------------------------------------
void setup()
{
	pinMode( LED_BUILTIN, OUTPUT );
	digitalWrite( LED_BUILTIN, HIGH );
	pinMode( GSM_RESET_PIN, OUTPUT );

	esp::init();

	Serial.begin( 115200 );

	GSM_BEGIN;
	GSM_RESET();
	delay( 3000 );

	if( at.sendATCommand( "AT", true ) ){
		ESP_DEBUG( "GSM INIT OK\n" );
	}else{
		ESP_DEBUG( "GSM INIT ERROR\n" );
		delay( 3000 );
		ESP.reset();
	}

	// sendATCommand( GSM_SMS_INIT, true );



	IPAddress apIP( 10, 10, 10, 10 );
	bool res = esp::wifi_init( DEVICE_NAME, apIP, apIP, IPAddress( 255, 255, 255, 0 ) );
	if( esp::flags.ap_mode ){
		//Captive portal redirect
		dnsServer.setErrorReplyCode( DNSReplyCode::NoError );
		dnsServer.addRecord( "*", apIP );
		dnsServer.start( DNS_PORT );
		//
	}
	
	esp::pageBuff = pageBuff;
	esp::addWebServerPages( &webServer, true, true, true );
	webServer.on( "/", handleRoot );
	webServer.begin();

	delay( 700 );

	ESP_DEBUG( "INIT OK\n" );
}

//-------------------------------------------------------------------------------
void loop()
{
	if( timer0.isInterrupt() ){
		timer0.confirmInerrupt();

		ESP_DEBUG( "--- TIMER [%d] ---\n", check_counter );

		if( !esp::flags.ap_mode ){
			if( !esp::isWiFiConnection() ){
				digitalWrite( LED_BUILTIN, HIGH );
				WiFi.reconnect();
			}else{
				digitalWrite( LED_BUILTIN, LOW );
			}
		}

		switch( ++check_counter ){
			case 1:		at.sendATCommand( "AT+CBC", false );	break;
			case 10:	at.sendATCommand( "AT+CREG?", false );	break;
			case 20:	at.sendATCommand( "AT+COPS?", false );	break;
			case 30:	at.sendATCommand( "AT+CSQ", false );	break;
			case 40:	check_counter = 0;	break;
			default: break;
		}
	}

	at.dataRead();
	webServer.handleClient();
	if( esp::flags.ap_mode ) dnsServer.processNextRequest();
}

