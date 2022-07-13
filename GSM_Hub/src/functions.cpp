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

//-------------------------------------------------------------------------------
bool GSM_responseProcess(void)
{
	if( at.isRing() ){
		at.sendATCommand( "ATH", false );
		// at.sendATCommand( "ATA", false ); //Answer
		return true;
	}

	if( at.isOK() ){
		ESP_DEBUG( ">:%s\n", at.getData() );
		
		// if( strcmp( at.getData(), GSM_SMS_INIT ) == 0 ){
		// 	init_flags.sms = 1;
		// }else if( strcmp( at.getData(), GSM_CONTYPE_INIT ) == 0 ){
		if( strcmp( at.getData(), GSM_CONTYPE_INIT ) == 0 ){
			// init_flags.contype = 1;
		}else if( strcmp( at.getData(), GSM_APN_INIT ) == 0 ){
			// init_flags.apn = 1;
		}else if( strcmp( at.getData(), GSM_USER_INIT ) == 0 ){
			// init_flags.user = 1;
		}else if( strcmp( at.getData(), GSM_PWD_INIT ) == 0 ){
			// init_flags.pwd = 1;
		// }else if( strcmp( at.getData(), GSM_PDP_DEFINE ) == 0 ){
		// 	init_flags.pdp = 1;
		// }else if( strcmp( at.getData(), GSM_PDP_ACTIVATE ) == 0 ){
		// 	init_flags.pdp_activ = 1;
		// }else if( strcmp( at.getData(), GSM_GPRS_INIT ) == 0 ){
		// 	init_flags.gprs = 1;
		// }else if( strcmp( at.getData(), GSM_GPRS_ATTACH ) == 0 ){
		// 	init_flags.gprs_attach = 1;
		// }else if( strcmp( at.getData(), GSM_HTTP_INIT ) == 0 ){
		// 	init_flags.http = 1;
		// }else if( strcmp( at.getData(), GSM_HTTPCID_INIT ) == 0 ){
		// 	init_flags.httpcid = 1;
		}else{
			// char* pointer = strchr( at.getData(), '\n' );
			if( at.isCommand( true ) ){
				ESP_DEBUG( ">:[CMD]:%s = %s\n", at.getCmd(), at.getValue() );
				if( strcmp( at.getCmd(), "+CSQ" ) == 0 ){
					char* value = at.getValue();
					char* comma = strchr( value, ',' );
					comma[ 0 ] = '\0';
					comma++;

					gsmData.rssi = atoi( value );
					gsmData.ber = atoi( comma );

					ESP_DEBUG( "RSSI: %s/%d BER: %s/%d\n", value, gsmData.rssi, comma, gsmData.ber );
				}else if( strcmp( at.getCmd(), "+CREG" ) == 0 ){
					char* value = at.getValue();
					char* comma = strchr( value, ',' );
					comma[ 0 ] = '\0';
					comma++;

					uint8_t n = atoi( value );
					if( n > 1 ) comma[ 1 ] = '\0';
					gsmData.reg = atoi( comma );

					ESP_DEBUG( "REG: %d\n", gsmData.reg );
				}else if( strcmp( at.getCmd(), "+COPS" ) == 0 ){
					char* mode = at.getValue();
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

					ESP_DEBUG( "OPERATOR: %s\n", gsmData.opLabel );
				}else if( strcmp( at.getCmd(), "+CBC" ) == 0 ){
					uint8_t bcs = 0;
					uint8_t bcl = 0;
					// uint8_t vol = 0;

					char* mode = at.getValue();
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

		at.resetBuffer();

		return true;
	}else if( at.isERROR() ){
		ESP_DEBUG( "[ERR]: %s\n", at.getData() );
		// if( strcmp( at.getData(), GSM_SMS_CHECK ) == 0 ){
		// 	sendATCommand( "AT+CMGDA=\"DEL ALL\"", false );
		// 	delay( 15000 );
		// }

		// sendATCommand( "AT+CMEE?", false );

		at.resetBuffer();
	}else if( at.isCommand() ){
		ESP_DEBUG( "[CMD]: %s = %s\n", at.getCmd(), at.getValue() );
		if( strcmp( at.getCmd(), "+CMTI" ) == 0 ){
			char* comma = strchr( at.getValue(), ',' ) + 1;
			uint8_t indx = atoi( comma );

			strcpy( gsm_data_buff, "AT+CMGR=" );itoa( indx, tmpVal, 10 );strcat( gsm_data_buff, tmpVal );
			at.sendATCommand( gsm_data_buff, false );
			// delay( 1500 );
			// sendATCommand( "AT+CMGDA=\"DEL ALL\"", false );
			// delay( 15000 );
			// strcpy( gsm_data_buff, "AT+CMGD=" );itoa( indx, tempValNumBuf, 10 );strcat( gsm_data_buff, tempValNumBuf );
			// sendATCommand( gsm_data_buff, false );
		}

		at.resetBuffer();
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


