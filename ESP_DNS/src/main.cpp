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
#endif
	LittleFS.begin();
	delay( 700 );
	

#ifdef __DEV
	Serial.println( "BOOTING..." );

	Dir root = LittleFS.openDir( "/" );
	while( root.next() ){
		File file = root.openFile("r");
		Serial.print( ": " );
		Serial.println( root.fileName() );
		file.close();
	}
#endif
	IPAddress apIP( 10, 10, 10, 10 );
	bool isClient = esp::isClient();
	bool res = esp::wifi_init( DEVICE_NAME, apIP, apIP, IPAddress( 255, 255, 255, 0 ) );
	if( res ){
		if( isClient ){
#ifdef __DEV
			Serial.println( "Wi-Fi Client mode" );
#endif
			dnsServer.setErrorReplyCode( DNSReplyCode::NonExistentDomain );
			dnsServer.addRecord( "test.lan", IPAddress( 192, 168, 1, 1 ) );
			dnsServer.addRecord( "test2.lan", IPAddress( 192, 168, 1, 1 ) );

			if( LittleFS.exists( RULES_FILE ) ){	
				File f = LittleFS.open( RULES_FILE, "r");
				uint8_t i = 0;
				uint8_t nameFlag = 1;
				char tmpNameBuff[ MAX_DNSNAME_LENGTH ];
				uint8_t tmpIPBuff[ 4 ];

				while( f.available() ){
					char sym;
					f.readBytes( &sym, 1 );

					if( nameFlag ){
						if( sym == '	' ){
							tmpNameBuff[ i++ ] = '\0';
							i = 0;
							nameFlag = 0;
							continue;
						}

						if( i >= MAX_DNSNAME_LENGTH ) continue;
						tmpNameBuff[ i++ ] = sym;
					}else{
						tmpIPBuff[ i++ ] = (uint8_t)sym;
						if( i == 4 ){
							i = 0;
							nameFlag = 1;
							dnsServer.addRecord( tmpNameBuff, IPAddress( tmpIPBuff ) );
						}
					}
				}
				f.close();
			}
		}else{
#ifdef __DEV
			Serial.println( "Wi-Fi AP mode" );
#endif
			//Captive portal redirect
			dnsServer.setErrorReplyCode( DNSReplyCode::NoError );
			dnsServer.addRecord( "*", apIP );
			//
		}
	}

	dnsServer.start( DNS_PORT );
	dnsServer.newRequest( new_DNS_request );



	esp::pageBuff = pageBuff;
	esp::addWebServerPages( &webServer, true, true );
	webServer.on( "/", handleRoot );
	webServer.on( "/index.js", handleIndexJS );
	webServer.on( "/index.css", handleIndexCSS );
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

