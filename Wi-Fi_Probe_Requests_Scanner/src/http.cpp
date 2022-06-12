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
	if( flags.apMode ){
		handleWifiConfig();
		return;
	}




	setNoCacheContent();

	strcpy( pageBuff, pageTop );
	strcat( pageBuff, "<title>ROOT</title>" );
	strcat( pageBuff, pageEndTop );
	strcat( pageBuff, "<h1>Index 2</h1>" );
	strcat( pageBuff, pageBottom );

	webServer.send ( 200, "text/html", pageBuff);
}

//-------------------------------------------------------------------------------
void handleWifiConfig(void)
{
	//-------------------------------------------------------------
	if( webServer.hasArg( "getAccess" ) ) flags.captivePortalAccess = 1;
	//-------------------------------------------------------------
	if( webServer.hostHeader() == "captive.apple.com" || webServer.hostHeader() == "clients3.google.com" ){
		if( flags.captivePortalAccess ){
			webServer.send( 200, "text/html", "<HTML><HEAD><TITLE>Success</TITLE></HEAD><BODY>Success</BODY></HTML>" );
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
				strcat( pageBuff, "<td>KEY:</td>" );
				strcat( pageBuff, "<td>" );
					strcat( pageBuff, "<input name='key'>" );
				strcat( pageBuff, "</td>" );
			strcat( pageBuff, "</tr>" );
			strcat( pageBuff, "<tr>" );
				strcat( pageBuff, "<td colspan='2' align='center'><input type='submit' value='Save & connect'></td>" );
			strcat( pageBuff, "</tr>" );
		strcat( pageBuff, "</table>" );
	strcat( pageBuff, "</form>" );



	if( flags.captivePortalAccess ){
		strcat( pageBuff, "<h1>Captive Portal</h1>" );
		strcat( pageBuff, "<a href=\"http://" );
		strcat( pageBuff, webServer.client().localIP().toString().c_str() );
		strcat( pageBuff, "/wifi\" target=\"_blank\">Open at Browser</a>" );
	}else{
		strcat( pageBuff, "<input type=\"button\" value=\"getAccess\" onClick=\"window.open( '/?getAccess' );\">" );
	}

	strcat( pageBuff, pageBottom );

	webServer.send ( 200, "text/html", pageBuff );
}

//-------------------------------------------------------------------------------

void CaptivePortalPage(void)
{
	if( flags.captivePortalAccess ){
		strcat( pageBuff, "<h1>Captive Portal</h1>" );
		strcat( pageBuff, "<a href=\"http://" );
		strcat( pageBuff, webServer.client().localIP().toString().c_str() );
		strcat( pageBuff, "/wifi\" target=\"_blank\">Open at Browser</a>" );
	}else{
		strcat( pageBuff, "<input type=\"button\" value=\"getAccess\" onClick=\"window.open( '/?getAccess' );\">" );
	}
}


//-------------------------------------------------------------------------------
void handleCaptivePortal(void)
{
	if( !flags.captivePortalAccess ){
		redirect();
		return;
	}else{
		strcpy( pageBuff, pageTop );
		strcat( pageBuff, "<title>CP</title>" );
		strcat( pageBuff, pageEndTop );
		CaptivePortalPage();
		strcat( pageBuff, pageBottom );
		webServer.send ( ( !flags.captivePortalAccess ) ? 200 : 204, "text/html", pageBuff );
	}
}

//-------------------------------------------------------------------------------
void handleNotFound(void)
{
	if( flags.apMode ){
		redirect();
		return;
	}

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
