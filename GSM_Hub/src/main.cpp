#include "main.h"
#include "functions.h"
#include <timer.h>
#include "http.h"

//-------------------------------------------------------------------------------
Timer timer0( 0, 1000 );
DNS_Server dnsServer;
ESP8266WebServer webServer( 80 );
ATCommand AT;
GSM_Data gsmData;
char tmpVal[ 10 ];
char pageBuff[ WEB_PAGE_BUFF_SIZE ];
char gsm_data_buff[ GSM_DATA_BUFF_SIZE ];
uint8_t check_counter					= 0;
long wtimer								= 0;

//-------------------------------------------------------------------------------
void setup()
{
	pinMode( LED_BUILTIN, OUTPUT );
	digitalWrite( LED_BUILTIN, HIGH );
	pinMode( GSM_RESET_PIN, OUTPUT );

	esp::init();

	GSM_BEGIN;
	GSM_RESET();
	delay( 3000 );

	sendATCommand( "AT", true );
	sendATCommand( GSM_SMS_INIT, true );



	IPAddress apIP( 10, 10, 10, 10 );
	bool res = esp::wifi_init( DEVICE_NAME, apIP, apIP, IPAddress( 255, 255, 255, 0 ) );
	if( res ){
		if( esp::isClient() ){
#ifdef __DEV
			Serial.println( "Wi-Fi Client mode" );
#endif
			dnsServer.setErrorReplyCode( DNSReplyCode::NonExistentDomain );
		}else{
#ifdef __DEV
			Serial.println( "Wi-Fi AP mode" );
#endif
			//Captive portal redirect
			dnsServer.setErrorReplyCode( DNSReplyCode::NoError );
			dnsServer.addRecord( "*", apIP );
			dnsServer.start( DNS_PORT );
			//
		}
	}
	
	esp::pageBuff = pageBuff;
	esp::addWebServerPages( &webServer, true, true, true );
	webServer.on( "/", handleRoot );
	webServer.begin();

	delay( 700 );
#ifdef __DEBUG
	Serial1.println( "INIT OK" );
#endif
}

//-------------------------------------------------------------------------------
void loop()
{
	if( timer0.isInterrupt() ){
		timer0.confirmInerrupt();

		if( !esp::flags.ap_mode ){
			if( !esp::isWiFiConnection() ){
				digitalWrite( LED_BUILTIN, HIGH );
				WiFi.reconnect();
			}else{
				digitalWrite( LED_BUILTIN, LOW );
			}
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
#ifdef __DEBUG
		Serial1.print( sym );
		Serial1.print( ":" ); Serial1.println( AT.getData() );
#endif
		GSM_responseProcess();
	}

	webServer.handleClient();
	
	if( esp::flags.ap_mode ){
		dnsServer.processNextRequest();
	}
}

