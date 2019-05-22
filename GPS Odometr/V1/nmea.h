#define SPEED_VALID 'A'
#define SPEED_NO_VALID 'V'

enum {
	NMEA_STR_TYPE_GPGSV,
	NMEA_STR_TYPE_GPGGA,
	NMEA_STR_TYPE_GPRMC,
	NMEA_STR_TYPE_GPVTG,
};

typedef struct {
	uint8_t sputniks;
	uint8_t myPosFound;
	int16_t altitude;
	uint8_t speed;
	uint8_t speedValid;
	uint8_t odometr5;
	uint8_t odometr4;
	uint8_t odometr3;
	uint8_t odometr2;
	uint8_t odometr1;
	uint16_t odometr;
} GPS_Data;

GPS_Data gps;

/*	##########################################################
##############################################################
########################################################	*/
uint8_t Hex(uint8_t i)
{
  uint8_t res;
  if( (i>('0'-1) && i<('9'+1)) || (i>('A'-1) && i<('Z'+1)) )
  {
    if(i<('9'+1)) res=(i-'0');
    else res=(i-'A'+10);
  }
  return res;
}
/*	##########################################################
##############################################################
########################################################	*/
uint8_t isGPGSV(const uint8_t* buff)
{
	if( buff[0] == 'G' && buff[1] == 'P' && buff[2] == 'G' && buff[3] == 'S' && buff[4] == 'V' ) return 1;
	return 0;
}

uint8_t isGPGGA(const uint8_t* buff)
{
	if( buff[0] == 'G' && buff[1] == 'P' && buff[2] == 'G' && buff[3] == 'G' && buff[4] == 'A' ) return 1;
	return 0;
}

uint8_t isGPRMC(const uint8_t* buff)
{
	if( buff[0] == 'G' && buff[1] == 'P' && buff[2] == 'R' && buff[3] == 'M' && buff[4] == 'C' ) return 1;
	return 0;
}

uint8_t isGPVTG(const uint8_t* buff)
{
	if( buff[0] == 'G' && buff[1] == 'P' && buff[2] == 'V' && buff[3] == 'T' && buff[4] == 'G' ) return 1;
	return 0;
}
/*	##########################################################
##############################################################
########################################################	*/
void nmea_parsGPGGA_pkt(const uint8_t* string)
{
	uint8_t dataCouter = 0;
	uint8_t symCounter = 0;
	uint8_t minusFlag = 0;

	while(*string != '\0'){
		if( *string == ',' ){
			symCounter = 0;
			dataCouter++;
			string++;
			continue;
		}
		
		if( *string == '.' ){
			symCounter = 10;
			string++;
			continue;
		}
		if( *string == '-' && symCounter == 0 ){
			minusFlag = 1;
			string++;
			continue;
		}

		switch ( dataCouter ){
			case 6: gps.myPosFound = Hex(*string); break;
			case 7:
				switch ( symCounter ){
					case 0: gps.sputniks = Hex(*string)<<4; break;
					case 1: gps.sputniks += Hex(*string); break;
				}
			break;
			case 9:
				switch ( symCounter ){
					case 0:
						if( minusFlag ){
							gps.altitude = 0 - Hex(*string);
						}else{
							gps.altitude = Hex(*string);
						}
						minusFlag = 0;
					break;
					case 1: gps.altitude *= 10; gps.altitude += Hex(*string); break;
					case 2: gps.altitude *= 10; gps.altitude += Hex(*string); break;
				}
			break;
		}
		symCounter++;
		string++;
	}
}

void nmea_parsGPRMC_pkt(const uint8_t* string)
{
	uint8_t dataCouter = 0;
	uint8_t symCounter = 0;
	uint8_t minusFlag = 0;

	gps.speedValid = SPEED_NO_VALID;

	while(*string != '\0'){
		if( *string == ',' ){
			symCounter = 0;
			dataCouter++;
			string++;
			continue;
		}
		
		if( *string == '.' ){
			symCounter = 10;
			string++;
			continue;
		}
		if( *string == '-' && symCounter == 0 ){
			minusFlag = 1;
			string++;
			continue;
		}

		switch ( dataCouter ){
			case 2: gps.speedValid = *string; break;
		}
		symCounter++;
		string++;
	}
}

void nmea_parsGPVTG_pkt(const uint8_t* string)
{
	uint8_t dataCouter = 0;
	uint8_t symCounter = 0;
	uint8_t minusFlag = 0;

	while(*string != '\0'){
		if( *string == ',' ){
			symCounter = 0;
			dataCouter++;
			string++;
			continue;
		}
		
		if( *string == '.' ){
			symCounter = 10;
			string++;
			continue;
		}
		if( *string == '-' && symCounter == 0 ){
			minusFlag = 1;
			string++;
			continue;
		}

		switch ( dataCouter ){
			case 7:
				switch ( symCounter ){
					case 0: gps.speed = Hex(*string); break;
					case 1: gps.speed *= 10; gps.speed += Hex(*string); break;
					case 2: gps.speed *= 10; gps.speed += Hex(*string); break;
				}
			break;
		}
		symCounter++;
		string++;
	}
}

void nmea_parsString(const uint8_t* buff)
{
	if( isGPGGA(buff) ){
		nmea_parsGPGGA_pkt(buff);
		return;
	}
	if( isGPRMC(buff) ){
		nmea_parsGPRMC_pkt(buff);
		return;
	}
	if( isGPVTG(buff) ){
		nmea_parsGPVTG_pkt(buff);
		return;
	}
}
