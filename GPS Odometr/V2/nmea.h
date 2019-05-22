#define SPEED_VALID 'A'
#define SPEED_NO_VALID 'V'

#define NMEA_BUFF_SIZE 512
#define NMEA_BUFF_MASK	NMEA_BUFF_SIZE - 1

#define NMEA_STR_TYPE_GPGGA 2
#define NMEA_STR_TYPE_GPGSV 3
#define NMEA_STR_TYPE_GPZDA 4
#define NMEA_STR_TYPE_GPRMC 5
#define NMEA_STR_TYPE_GPGLL 6
#define NMEA_STR_TYPE_GPVTG 7
#define NMEA_STR_TYPE_GPTXT 8

typedef struct {
	uint8_t sputniks;
	uint8_t myPosFound;
	uint8_t speedValid;
	int16_t altitude;
	uint8_t altitudeDrob;
	uint8_t speed;
	uint8_t speedDrob;

	uint16_t odometr;
	uint8_t odometrDrob;
} GPS_Data;

typedef struct {
	uint8_t sputniks[2];
	uint8_t altitude[5];
	uint8_t speed[7];
} GPS_RAW_Data;

GPS_Data gps;
GPS_RAW_Data gpsRaw;

/*	##########################################################
##############################################################
########################################################	*/
void nmea_parsing( uint8_t sym )
{
	static uint8_t counter = 0;
	static uint8_t dataCouter = 0;
	static uint8_t type = 0;
	uint8_t find = false;
	static uint8_t rxBuff[15];
	uint8_t i;
	
	if( sym == '$' ){
		type = 0;
		sym = ',';
	}
	if( sym == '$' || sym == '\r' || sym == '\0' || sym == '\n' ) return;

	rxBuff[counter] = sym;
	if( type == NMEA_STR_TYPE_GPGGA ) uart_send( sym );

	if( sym == ',' ){
		dataCouter++;
		if( counter == 5 ){
			if( rxBuff[0] == 'G' && rxBuff[1] == 'P' && rxBuff[2] == 'G' && rxBuff[3] == 'G' && rxBuff[4] == 'A' ){
				type = NMEA_STR_TYPE_GPGGA;
				dataCouter = 0;
				find = true;
			}
			if( rxBuff[0] == 'G' && rxBuff[1] == 'P' && rxBuff[2] == 'G' && rxBuff[3] == 'S' && rxBuff[4] == 'V' ){
				type = NMEA_STR_TYPE_GPGSV;
				dataCouter = 0;
				find = true;
			}
			if( rxBuff[0] == 'G' && rxBuff[1] == 'P' && rxBuff[2] == 'Z' && rxBuff[3] == 'D' && rxBuff[4] == 'A' ){
				type = NMEA_STR_TYPE_GPZDA;
				dataCouter = 0;
				find = true;
			}
			if( rxBuff[0] == 'G' && rxBuff[1] == 'P' && rxBuff[2] == 'R' && rxBuff[3] == 'M' && rxBuff[4] == 'C' ){
				type = NMEA_STR_TYPE_GPRMC;
				dataCouter = 0;
				find = true;
			}
			if( rxBuff[0] == 'G' && rxBuff[1] == 'P' && rxBuff[2] == 'G' && rxBuff[3] == 'L' && rxBuff[4] == 'L' ){
				type = NMEA_STR_TYPE_GPGLL;
				dataCouter = 0;
				find = true;
			}
			if( rxBuff[0] == 'G' && rxBuff[1] == 'P' && rxBuff[2] == 'V' && rxBuff[3] == 'T' && rxBuff[4] == 'G' ){
				type = NMEA_STR_TYPE_GPVTG;
				dataCouter = 0;
				find = true;
			}
			if( rxBuff[0] == 'G' && rxBuff[1] == 'P' && rxBuff[2] == 'T' && rxBuff[3] == 'X' && rxBuff[4] == 'T' ){
				type = NMEA_STR_TYPE_GPTXT;
				dataCouter = 0;
				find = true;
			}
			if( find ){
				counter = 0;
				return;
			}
		}
		if( type == NMEA_STR_TYPE_GPGGA && dataCouter == 1 ){
			if( counter != 10 ) type = 0;

		}
		if( type == NMEA_STR_TYPE_GPGGA && dataCouter == 6 ){
			if( counter == 1 ){
				gps.myPosFound = symToHex( rxBuff[0] );
			}else{
				gps.myPosFound = 0x30;
				type = 0;
			}
		}
		if( type == NMEA_STR_TYPE_GPGGA && dataCouter == 7 ){
			if( counter == 2 ){
				if( rxBuff[0] == '.' ) rxBuff[0] = 0x30;
				gpsRaw.sputniks[0] = rxBuff[0];
				gpsRaw.sputniks[1] = rxBuff[1];
			}else{
				gpsRaw.sputniks[0] = '\0';
			}
		}
		if(type == NMEA_STR_TYPE_GPGGA && dataCouter == 9) {
			for( i = 0; i < counter; i++ ) gpsRaw.altitude[i] = rxBuff[i];
			gpsRaw.altitude[i] = '\0';
		}
		if(type == NMEA_STR_TYPE_GPRMC && dataCouter == 2) {
			gps.speedValid = SPEED_NO_VALID;
			if( counter == 1 ) gps.speedValid = rxBuff[0];
		}
		if(type == NMEA_STR_TYPE_GPVTG && dataCouter == 7) {
			for( i = 0; i < counter; i++ ) gpsRaw.speed[i] = rxBuff[i];
			gpsRaw.speed[i] = '\0';
		}
		counter = 0;
		return;
	}
	counter++;
}

void nmea_dataCheck()
{
	gps.sputniks = strToUChar( gpsRaw.sputniks, 2 );
	gps.altitudeDrob = strToDrob( gpsRaw.altitude );
	gps.altitude = strToShort2( gpsRaw.altitude );
	gps.speed = strToUChar( gpsRaw.speed, 3 );
	gps.speedDrob = strToDrob( gpsRaw.speed );
}
