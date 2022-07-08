#include "functions.h"
#include "main.h"

//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
void GSM_RESET()
{
	digitalWrite( GSM_RESET_PIN, LOW );	delay( 300 );
	digitalWrite( GSM_RESET_PIN, HIGH );delay( 300 );
}

//-------------------------------------------------------------------------------
bool sendATCommand(const char* cmd, bool waiting, bool process)
{
#ifdef __DEBUG
		Serial1.print( ">:" ); Serial1.println( cmd );
#endif
	Serial.println( cmd );

	if( !waiting ) return false;

	AT.resetBuffer();
	wtimer = millis();

	while( !AT.isOK() && !AT.isERROR() && ( wtimer + 37000 > millis() ) ){
		if( GSM_AVAILABLE ){
			AT.addSymbol( GSM_READ );
		}
	}
	
	if( !AT.isOK() && !AT.isERROR() ){
		while( GSM_AVAILABLE ){
			AT.addSymbol( GSM_READ );
		}
	}

	if( !process ) return true;

	return GSM_responseProcess();
}

//-------------------------------------------------------------------------------
bool GSM_responseProcess(void)
{
	if( AT.isOK() ){
#ifdef __DEBUG
		Serial1.print( "[OK]:" ); Serial1.println( AT.getData() );
#endif
		
		// if( strcmp( AT.getData(), GSM_SMS_INIT ) == 0 ){
		// 	init_flags.sms = 1;
		// }else if( strcmp( AT.getData(), GSM_CONTYPE_INIT ) == 0 ){
		if( strcmp( AT.getData(), GSM_CONTYPE_INIT ) == 0 ){
			// init_flags.contype = 1;
		}else if( strcmp( AT.getData(), GSM_APN_INIT ) == 0 ){
			// init_flags.apn = 1;
		}else if( strcmp( AT.getData(), GSM_USER_INIT ) == 0 ){
			// init_flags.user = 1;
		}else if( strcmp( AT.getData(), GSM_PWD_INIT ) == 0 ){
			// init_flags.pwd = 1;
		// }else if( strcmp( AT.getData(), GSM_PDP_DEFINE ) == 0 ){
		// 	init_flags.pdp = 1;
		// }else if( strcmp( AT.getData(), GSM_PDP_ACTIVATE ) == 0 ){
		// 	init_flags.pdp_activ = 1;
		// }else if( strcmp( AT.getData(), GSM_GPRS_INIT ) == 0 ){
		// 	init_flags.gprs = 1;
		// }else if( strcmp( AT.getData(), GSM_GPRS_ATTACH ) == 0 ){
		// 	init_flags.gprs_attach = 1;
		// }else if( strcmp( AT.getData(), GSM_HTTP_INIT ) == 0 ){
		// 	init_flags.http = 1;
		// }else if( strcmp( AT.getData(), GSM_HTTPCID_INIT ) == 0 ){
		// 	init_flags.httpcid = 1;
		}else{
			// char* pointer = strchr( AT.getData(), '\n' );
			if( AT.isCommand( true ) ){
#ifdef __DEBUG
				Serial1.print( "[CMD OK]:" ); Serial1.print( AT.getCmd() ); Serial1.print( " = " ); Serial1.println( AT.getValue() );
#endif

				if( strcmp( AT.getCmd(), "+CSQ" ) == 0 ){
					char* value = AT.getValue();
					char* comma = strchr( value, ',' );
					comma[ 0 ] = '\0';
					comma++;

					gsmData.rssi = atoi( value );
					gsmData.ber = atoi( comma );
#ifdef __DEBUG
					Serial1.print( "RSSI:" ); Serial1.print( value ); Serial1.print( "/" ); Serial1.print( gsmData.rssi );
					Serial1.print( " BER:" ); Serial1.print( comma ); Serial1.print( "/" ); Serial1.println( gsmData.ber );
#endif
				}else if( strcmp( AT.getCmd(), "+CREG" ) == 0 ){
					char* value = AT.getValue();
					char* comma = strchr( value, ',' );
					comma[ 0 ] = '\0';
					comma++;

					uint8_t n = atoi( value );
					if( n > 1 ) comma[ 1 ] = '\0';
					gsmData.reg = atoi( comma );
#ifdef __DEBUG
					Serial1.print( "REG:" ); Serial1.println( gsmData.reg );
#endif
				}else if( strcmp( AT.getCmd(), "+COPS" ) == 0 ){
					char* mode = AT.getValue();
					char* format = strchr( mode, ',' );
					if( format != NULL ){
						format[ 0 ] = '\0';
						format++;
						char* opLable = strchr( format, ',' );
						if( opLable != NULL ){
							opLable[ 0 ] = '\0';
							opLable++;
							if( opLable[ 0 ] == '"' ) opLable++;
							uint8_t len = strlen( opLable );
							if( len ){
								len--;
								if( opLable[ len ] == '"' ) opLable[ len ] = '\0';
							}
							strcpy( gsmData.opLabel, opLable );
						}
					}
#ifdef __DEBUG
					Serial1.print( "OPERATOR:" ); Serial1.println( gsmData.opLabel );
#endif
				}else if( strcmp( AT.getCmd(), "+CBC" ) == 0 ){
					uint8_t bcs = 0;
					uint8_t bcl = 0;
					// uint8_t vol = 0;

					char* mode = AT.getValue();
					char* prz = strchr( mode, ',' );
					if( prz != NULL ){
						prz[ 0 ] = '\0';
						prz++;
						bcl = atoi( prz );
						// char* voltage = strchr( prz, ',' );
						// if( voltage != NULL ){
						// 	voltage[ 0 ] = '\0';
						// 	voltage++;
						// 	vol = atoi( voltage );
						// }
					}

					bcs = atoi( mode );
				}
			}
		}

		AT.resetBuffer();

		return true;
	}else if( AT.isERROR() ){
#ifdef __DEBUG
		Serial1.print( "[ERR]:" ); Serial1.println( AT.getData() );
#endif
		// if( strcmp( AT.getData(), GSM_SMS_CHECK ) == 0 ){
		// 	sendATCommand( "AT+CMGDA=\"DEL ALL\"", false );
		// 	delay( 15000 );
		// }

		// sendATCommand( "AT+CMEE?", false );

		AT.resetBuffer();
	}else if( AT.isCommand() ){
#ifdef __DEBUG
		Serial1.print( "[CMD]:" ); Serial1.print( AT.getCmd() ); Serial1.print( " = " ); Serial1.println( AT.getValue() );
#endif
		if( strcmp( AT.getCmd(), "+CMTI" ) == 0 ){
			char* comma = strchr( AT.getValue(), ',' ) + 1;
			uint8_t indx = atoi( comma );

			strcpy( gsm_data_buff, "AT+CMGR=" );itoa( indx, tmpVal, 10 );strcat( gsm_data_buff, tmpVal );
			sendATCommand( gsm_data_buff, false );
			// delay( 1500 );
			// sendATCommand( "AT+CMGDA=\"DEL ALL\"", false );
			// delay( 15000 );
			// strcpy( gsm_data_buff, "AT+CMGD=" );itoa( indx, tempValNumBuf, 10 );strcat( gsm_data_buff, tempValNumBuf );
			// sendATCommand( gsm_data_buff, false );
		}

		AT.resetBuffer();
		return true;
	}

	return false;
}
//-------------------------------------------------------------------------------
// bool sendATCommand(char* cmd, bool waiting)
// {
// 	return sendATCommand( cmd, waiting );
// }

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------


