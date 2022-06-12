#include "http.h"
#include "main.h"
#include "functions.h"

//-------------------------------------------------------------------------------
const char pageTop[] = "<!DOCTYPE HTML><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><meta charset=\"utf-8\"/>";
const char pageEndTop[] = "</head><body><hr size=\"1\" color=\"#ff0ff0\">";
const char pageBottom[] = "<hr size=\"1\" color=\"#ff0000\"></body></html>";
char pageBuff[ 2048 ];

//-------------------------------------------------------------------------------
void setNoCacheContent(void)
{
	webServer.sendHeader( "Cache-Control", "no-cache, no-store, must-revalidate" );
	webServer.sendHeader( "Pragma", "no-cache" );
	webServer.sendHeader( "Expires", "-1" );
}

//-------------------------------------------------------------------------------
void redirect(void)
{
	webServer.sendHeader( "Location", String( "http://" ) + webServer.client().localIP().toString(), true);
	webServer.send ( 302, "text/plain", "" );
	webServer.client().stop();
}

//-------------------------------------------------------------------------------
void handleRoot(void)
{
	//-------------------------------------------------------------
	if( webServer.hasArg( "close" ) ) flags.access = 0;
	if( webServer.hasArg( "open" ) ) flags.access = 1;
	//-------------------------------------------------------------




	setNoCacheContent();

	strcpy( pageBuff, pageTop );
	strcat( pageBuff, "<title>ROOT</title>" );
	strcat( pageBuff, pageEndTop );

	strcat( pageBuff, "<h1>Система запирания</h1>" );
	strcat( pageBuff, "<center>" );
	strcat( pageBuff, "<input type=button value=Закрыть onClick=location.href='?close'>" );
	strcat( pageBuff, "<input type=button value=Открыть onClick=location.href='?open'>" );
	strcat( pageBuff, "</center>" );

	strcat( pageBuff, pageBottom );

	webServer.send ( 200, "text/html", pageBuff);
}

//-------------------------------------------------------------------------------
void handleWifiConfig(void)
{
	if( !esp::checkWebAuth( &webServer, CONFIGURE_LOGIN, CONFIGURE_PASSWORD, CONFIGURE_REALM, "authFail" ) ){
		return;
	}
	//-------------------------------------------------------------
	if( webServer.hasArg( "ssid" ) && webServer.hasArg( "key" ) ){
		if( webServer.arg( "ssid" ).length() > 0 ){
			if( !esp::saveAPconfig( webServer.arg( "ssid" ).c_str(), webServer.arg( "key" ).c_str() ) ){
				webServer.client().write( "ERROR" );
				webServer.client().stop();
			}else{
				webServer.send ( 200, "text/html", "OK" );
				wifi_AP_init();
			}
			return;
		}
	}
	//-------------------------------------------------------------
	setNoCacheContent();

	strcpy( pageBuff, pageTop );
	strcat( pageBuff, "<title>Wi-Fi STATION Settings</title>" );
	strcat( pageBuff, pageEndTop );
	


	strcat( pageBuff, "<form action='/wifi' method='post'>" );
		strcat( pageBuff, "<table style=\"width: 300px; margin: auto;\">" );
			strcat( pageBuff, "<tr>" );
				strcat( pageBuff, "<td>SSID:</td>" );
				strcat( pageBuff, "<td>" );
					strcat( pageBuff, "<input name='ssid' value='" );
					strcat( pageBuff, DEVICE_NAME );
					strcat( pageBuff, "'>" );
				strcat( pageBuff, "</td>" );
			strcat( pageBuff, "</tr>" );
			strcat( pageBuff, "<tr>" );
				strcat( pageBuff, "<td>KEY:</td>" );
				strcat( pageBuff, "<td>" );
					strcat( pageBuff, "<input name='key'>" );
				strcat( pageBuff, "</td>" );
			strcat( pageBuff, "</tr>" );
			strcat( pageBuff, "<tr>" );
				strcat( pageBuff, "<td colspan='2' align='center'><input type='submit' value='Save & Submit'></td>" );
			strcat( pageBuff, "</tr>" );
		strcat( pageBuff, "</table>" );
	strcat( pageBuff, "</form>" );



	strcat( pageBuff, pageBottom );

	webServer.send ( 200, "text/html", pageBuff );
}

//-------------------------------------------------------------------------------
void handleNotFound(void)
{
	strcpy( pageBuff, pageTop );
	strcat( pageBuff, "<title>Not found</title>" );
	strcat( pageBuff, pageEndTop );
	strcat( pageBuff, "<h1>404 Not found</h1>" );
	strcat( pageBuff, pageBottom );

	webServer.send ( 404, "text/html", pageBuff);
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
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
