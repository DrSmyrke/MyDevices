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
uint8_t wifi_channel;
uint8_t unlockCounter;
uint8_t secCounter;


//-------------------------------------------------------------------------------
void setup()
{
	flags.apMode								= 0;
	flags.timer									= 0;
	flags.captivePortalAccess					= 0;
	flags.sniffing								= 0;
	flags.unlocked								= 0;
	wifi_channel								= 1;
	unlockCounter								= 0;
	secCounter									= 0;

	// LittleFS.begin();
	Serial.begin( 9600 );

	pinMode( LED_BUILTIN, OUTPUT );

	delay( 100 );

	// if( is_clientMode() ){
	// 	wifi_STA_init();
	// }else{
	// 	flags.apMode							= 1;
	// 	wifi_AP_init();
	// }

	timer_init();

	// webServer.on( "/", handleRoot );
	// webServer.on( "/wifi", handleWifiConfig );
	// //Captive portal
	// if( flags.apMode ){
	// 	webServer.on( "/fwlink", handleCaptivePortal );
	// 	webServer.on( "/generate_204", handleCaptivePortal );
	// 	webServer.on( "/favicon.ico", handleCaptivePortal );
	// }
	// webServer.onNotFound( handleNotFound );
	// webServer.begin();

	delay( 100 );

	wifi_sniffer_init();
}

//-------------------------------------------------------------------------------
void loop()
{
	
	if( flags.timer ){
		flags.timer = 0;

		if( secCounter ){
			secCounter--;
		}else{
			secCounter = 10;

			if( flags.unlocked ){
				if( unlockCounter > 0 ){
					unlockCounter--;
				}else{
					flags.unlocked = 0;
				}
			}
		}

		// if( !flags.apMode ){
		// 	if( !WiFiConnectionState() ) WiFi.reconnect();
		// 	// ArduinoOTA.handle();
		// }

		if( flags.sniffing ){
			wifi_set_channel( wifi_channel );
			wifi_channel = ( wifi_channel % WIFI_CHANNEL_MAX ) + 1;
		}

		
		
		digitalWrite( LED_BUILTIN, !flags.unlocked );
	}

	if( flags.apMode ){
		webServer.handleClient();
		dnsServer.processNextRequest();
	}
}

