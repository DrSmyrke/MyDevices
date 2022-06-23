#include "functions.h"
#include "main.h"

//-------------------------------------------------------------------------------
void DNS_request(const char* request, const uint8_t* ip)
{
#ifdef __DEV
	Serial.print( "DNS: " );
	Serial.print( request );
	if( ip != nullptr ){
		Serial.print( " -> " );
		utoa( ip[ 0 ], tmpVal, 10 ); Serial.print( tmpVal ); Serial.print( '.' );
		utoa( ip[ 1 ], tmpVal, 10 ); Serial.print( tmpVal ); Serial.print( '.' );
		utoa( ip[ 2 ], tmpVal, 10 ); Serial.print( tmpVal ); Serial.print( '.' );
		utoa( ip[ 3 ], tmpVal, 10 ); Serial.print( tmpVal );
	}
	Serial.println();
#endif
}

//-------------------------------------------------------------------------------
void read_rules(void)
{
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
}

//-------------------------------------------------------------------------------
void save_rules(void)
{
	File f = LittleFS.open( RULES_FILE, "w");

	if( f ){
		uint8_t i = 0;
		dnsServer.resetRulesIndex();
		while( dnsServer.nextRule() ){
			if( i > 0 ) strcat( pageBuff, ", " );

			const char* dname = dnsServer.getDomainName();
			const uint8_t* ip = dnsServer.getResolvedIP();

			f.write( dname, strlen( dname ) );
			f.write( (char)'	' );
			utoa( ip[ 0 ], tmpVal, 10 );  f.write( tmpVal, strlen( tmpVal ) );
			utoa( ip[ 1 ], tmpVal, 10 );  f.write( tmpVal, strlen( tmpVal ) );
			utoa( ip[ 2 ], tmpVal, 10 );  f.write( tmpVal, strlen( tmpVal ) );
			utoa( ip[ 3 ], tmpVal, 10 );  f.write( tmpVal, strlen( tmpVal ) );
		}
		
		f.close();
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
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------


