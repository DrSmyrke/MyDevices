#include "http.h"
#include "main.h"
#include "functions.h"


//-------------------------------------------------------------------------------
void handleRoot(void)
{
	esp::webSendFile( &webServer, "/index.html", "text/html" );
}

//-------------------------------------------------------------------------------
void handleRules(void)
{
	esp::setNoCacheContent( &webServer );

	//-------------------------------------------------------------
	if( webServer.hasArg( "setRule" ) && webServer.hasArg( "ip" ) ){
		if( webServer.arg( "setRule" ) != "" && webServer.arg( "ip" ) != "" ){
			IPAddress ip;
			ip.fromString( webServer.arg( "ip" ) );
			dnsServer.addRecord( webServer.arg( "setRule" ).c_str(), ip );
			save_rules();
			webServer.send ( 200, "text/html", "OK" );
		}else{
			webServer.send ( 200, "text/html", "ERROR" );
		}
		return;
	}
	//-------------------------------------------------------------

	strcpy( pageBuff, "{" );
	//-------------------------------------------------------------
	uint8_t i = 0;
	dnsServer.resetRulesIndex();
	while( dnsServer.nextRule() ){
		if( i > 0 ) strcat( pageBuff, ", " );

		const char* dname = dnsServer.getDomainName();
		const uint8_t* ip = dnsServer.getResolvedIP();

		strcat( pageBuff, "\"" ); utoa( i++, tmpVal, 10 ); strcat( pageBuff, tmpVal ); strcat( pageBuff, "\": {" );
			strcat( pageBuff, "\"ip\": \"" );
				utoa( ip[ 0 ], tmpVal, 10 ); strcat( pageBuff, tmpVal ); strcat( pageBuff, "." );
				utoa( ip[ 1 ], tmpVal, 10 ); strcat( pageBuff, tmpVal ); strcat( pageBuff, "." );
				utoa( ip[ 2 ], tmpVal, 10 ); strcat( pageBuff, tmpVal ); strcat( pageBuff, "." );
				utoa( ip[ 3 ], tmpVal, 10 ); strcat( pageBuff, tmpVal );
			strcat( pageBuff, "\"" );
			strcat( pageBuff, ", \"domain\": \"" ); strcat( pageBuff, dname ); strcat( pageBuff, "\"" );

		strcat( pageBuff, "}" );
	}
	//-------------------------------------------------------------
	strcat( pageBuff, "}" );
	webServer.send ( 200, "application/json", pageBuff );
}

//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

