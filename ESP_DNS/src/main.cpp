#include "main.h"
#include "functions.h"
#include <timer.h>
#include "http.h"

//-------------------------------------------------------------------------------
Timer timer0( 0, 1000 );
DNS_Server dnsServer;
ESP8266WebServer webServer( 80 );
MainFlags flags;
char tmpVal[ 10 ];
char pageBuff[ WEB_PAGE_BUFF_SIZE ];
IPAddress localIP( 0, 0, 0, 0 );

//------------ FUNCTIONS --------------------------------------------------------
void new_DNS_request(const char* dname, const uint8_t* ip)
{
	Serial.print( "DNS Request: " );
	Serial.print( dname );
	Serial.print( " [" );
	for( uint8_t i = 0; i < 4; i++ ){
		if( i > 0 ) Serial.print( '.' );
		Serial.print( ip[ i ] );
	}
	Serial.print( " ]" );
	Serial.println();
}

//-------------------------------------------------------------------------------


//-------------------------------------------------------------------------------
void setup()
{
#ifdef __DEV
	Serial.begin( 9600 );
	Serial.println( "BOOTING..." );
#endif

	esp::init();

	IPAddress apIP( 10, 10, 10, 10 );
	bool isClient = esp::isClient();
	bool res = esp::wifi_init( DEVICE_NAME, apIP, apIP, IPAddress( 255, 255, 255, 0 ) );
	if( res ){
		if( isClient ){
#ifdef __DEV
			Serial.println( "Wi-Fi Client mode" );
#endif
			dnsServer.setErrorReplyCode( DNSReplyCode::NonExistentDomain );
			read_rules();
		}else{
#ifdef __DEV
			Serial.print( "Wi-Fi AP mode " );
			Serial.print("IP: "); Serial.println( WiFi.softAPIP().toString() );
#endif
			//Captive portal redirect
			dnsServer.setErrorReplyCode( DNSReplyCode::NoError );
			dnsServer.addRecord( "*", WiFi.softAPIP() );
			//
		}
	}

	dnsServer.start( DNS_PORT );
	dnsServer.newRequest( new_DNS_request );



	esp::pageBuff = pageBuff;
	esp::addWebServerPages( &webServer, true, true, true );
	webServer.on( "/", handleRoot );
	webServer.on( "/rules", handleRules );
	webServer.begin();

	delay( 700 );
#ifdef __DEV
	Serial.println( "INIT OK" );
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

		if( localIP != WiFi.localIP() ){
			localIP = WiFi.localIP();
#ifdef __DEV
			Serial.print("New IP address: "); Serial.println( localIP.toString() );
#endif
		}
	}

	webServer.handleClient();
	dnsServer.processNextRequest();
}

