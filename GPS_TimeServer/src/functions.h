#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

bool WiFiConnectionState(void)
{
	bool state = false;
	if( WiFi.status() == WL_CONNECTED ) state = true;
	if( WiFi.status() == WL_CONNECTED && WiFi.localIP().toString() == "0.0.0.0" ) state = false;
	return state;
}

void wifi_init(void)
{
	WiFi.hostname( DEV_NAME );
	WiFi.softAPdisconnect( true );
	WiFi.mode( WiFiMode_t::WIFI_STA );
	
	WiFi.begin( WIFI_NETWORK, WIFI_NETWORK_KEY );
	WiFi.softAPdisconnect( true );

	delay( 100 );
}

#endif // __FUNCTIONS_H__