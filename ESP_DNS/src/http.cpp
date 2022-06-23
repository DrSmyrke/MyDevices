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

			String dname = webServer.arg( "setRule" );
			uint8_t i;
			for( i = 0; i < dname.length(); i++ ){
				pageBuff[ i ] = dname.charAt( i );
			}
			pageBuff[ i ] = '\0';
			dnsServer.addRecord( pageBuff, ip );
#ifdef __DEV
			Serial.print( "NEW RULE [" ); Serial.print( ip.toString() ); Serial.print( "] " ); Serial.println( pageBuff );
			if( ip.isV4() ) Serial.println( "V4" );
#endif
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
	strcat( pageBuff, "\"rules\": [" );
	uint8_t i = 0;
	dnsServer.resetRulesIndex();
	while( dnsServer.nextRule() ){
		if( i++ > 0 ) strcat( pageBuff, ", " );

		const char* dname = dnsServer.getDomainName();
		const uint8_t* ip = dnsServer.getResolvedIP();

#ifdef __DEV
		Serial.print( "HTTP RULE [" );
		utoa( ip[ 0 ], tmpVal, 10 ); Serial.print( tmpVal ); Serial.print( "." );
		utoa( ip[ 1 ], tmpVal, 10 ); Serial.print( tmpVal ); Serial.print( "." );
		utoa( ip[ 2 ], tmpVal, 10 ); Serial.print( tmpVal ); Serial.print( "." );
		utoa( ip[ 3 ], tmpVal, 10 ); Serial.print( tmpVal );
		Serial.print( "] " ); Serial.println( dname );
#endif

		// strcat( pageBuff, "\"" ); utoa( i++, tmpVal, 10 ); strcat( pageBuff, tmpVal ); strcat( pageBuff, "\": {" );
		strcat( pageBuff, "{" );
			strcat( pageBuff, "\"ip\": \"" );
				utoa( ip[ 0 ], tmpVal, 10 ); strcat( pageBuff, tmpVal ); strcat( pageBuff, "." );
				utoa( ip[ 1 ], tmpVal, 10 ); strcat( pageBuff, tmpVal ); strcat( pageBuff, "." );
				utoa( ip[ 2 ], tmpVal, 10 ); strcat( pageBuff, tmpVal ); strcat( pageBuff, "." );
				utoa( ip[ 3 ], tmpVal, 10 ); strcat( pageBuff, tmpVal );
			strcat( pageBuff, "\"" );
			strcat( pageBuff, ", \"domain\": \"" ); strcat( pageBuff, dname ); strcat( pageBuff, "\"" );

		strcat( pageBuff, "}" );
	}
	strcat( pageBuff, "]" );
	strcat( pageBuff, ", \"version\": \"" ); strcat( pageBuff, FIRMWARE_SECOND_VERSION ); strcat( pageBuff, "\"" );
	//-------------------------------------------------------------
	strcat( pageBuff, "}" );
	webServer.send ( 200, "application/json", pageBuff );
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

