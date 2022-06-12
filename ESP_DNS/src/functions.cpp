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

