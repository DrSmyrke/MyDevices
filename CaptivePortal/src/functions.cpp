#include "functions.h"
#include "main.h"

//-------------------------------------------------------------------------------
bool WiFiConnectionState(void)
{
	bool state = false;
	if( WiFi.status() == WL_CONNECTED ) state = true;
	if( WiFi.status() == WL_CONNECTED && WiFi.localIP().toString() == "0.0.0.0" ) state = false;
	return state;
}

//-------------------------------------------------------------------------------
void wifi_AP_init(void)
{
	WiFi.hostname( DEVICE_NAME );
	WiFi.disconnect();

	WiFi.mode( WiFiMode_t::WIFI_AP );
	WiFi.softAPConfig( apIP, apIP, apMASK );
	bool res = WiFi.softAP( DEVICE_NAME, "1234567890" );

	delay( 600 );

	if( res ){
		dnsServer.setErrorReplyCode( DNSReplyCode::NoError );
		dnsServer.start( 53, "*", apIP );
	}
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------


