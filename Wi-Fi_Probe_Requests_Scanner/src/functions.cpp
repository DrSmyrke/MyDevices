#include "functions.h"
#include "main.h"

//-------------------------------------------------------------------------------
uint8_t default_mac[WIFI_MAC_LEN] = {0x00,0x00,0x00,0x00,0x00,0x00};
uint8_t friendmac[WIFI_MAC_LEN] = {0xC4,0x61,0x8B,0x66,0x65,0xD0};
uint8_t friendmac2[WIFI_MAC_LEN] = {0xFA,0x82,0x8A,0x94,0xDA,0x75};

//-------------------------------------------------------------------------------
bool WiFiConnectionState(void)
{
	bool state = false;
	if( WiFi.status() == WL_CONNECTED ) state = true;
	if( WiFi.status() == WL_CONNECTED && WiFi.localIP().toString() == "0.0.0.0" ) state = false;
	return state;
}

//-------------------------------------------------------------------------------
bool is_clientMode(void)
{
	return ( LittleFS.exists( "/clientMode" ) ) ? true : false;
}

//-------------------------------------------------------------------------------
void wifi_STA_init(void)
{
	char ssid[ 32 ];
	char skey[ 32 ];
	uint8_t i = 0;

	WiFi.hostname( DEVICE_NAME );
	WiFi.softAPdisconnect( true );

	WiFi.mode( WiFiMode_t::WIFI_STA );

	if( LittleFS.exists( "/client" ) ){
		File f = LittleFS.open( "/client", "r");
		if( f ){
			bool first = true;
			while( f.available() ){
				char sym;
				f.readBytes( &sym, 1 );
				if( first ){
					if( sym == '\n' ){
						first = false;
						ssid[ i ] = '\0';
						i = 0;
						continue;
					}
					ssid[ i++ ] = sym;
				}else{
					if( sym == '\n' ){
						ssid[ i ] = '\0';
						break;
					}
					skey[ i++ ] = sym;
				}
			}
			f.close();
		}
	}

	WiFi.begin( ssid, skey );

	delay( 5000 );

	if( WiFi.waitForConnectResult() != WL_CONNECTED ){
		delay( 1000 );
		ESP.restart();
	}

	// ArduinoOTA.onStart([]() {
	// 	String type;
	// 	if (ArduinoOTA.getCommand() == U_FLASH) {
	// 		type = "sketch";
	// 	} else {  // U_FS
	// 		type = "filesystem";
	// 	}

	// 	// NOTE: if updating FS this would be the place to unmount FS using FS.end()
	// 	Serial.println("Start updating " + type);
	// });
	// ArduinoOTA.onEnd([]() {
	// 	Serial.println("\nEnd");
	// });
	// ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
	// 	Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
	// });
	// ArduinoOTA.onError([](ota_error_t error) {
	// 	Serial.printf("Error[%u]: ", error);
	// 	if (error == OTA_AUTH_ERROR) {
	// 		Serial.println("Auth Failed");
	// 	} else if (error == OTA_BEGIN_ERROR) {
	// 		Serial.println("Begin Failed");
	// 	} else if (error == OTA_CONNECT_ERROR) {
	// 		Serial.println("Connect Failed");
	// 	} else if (error == OTA_RECEIVE_ERROR) {
	// 		Serial.println("Receive Failed");
	// 	} else if (error == OTA_END_ERROR) {
	// 		Serial.println("End Failed");
	// 	}
	// });
	// ArduinoOTA.begin();
}

//-------------------------------------------------------------------------------
void wifi_AP_init(void)
{
	WiFi.hostname( DEVICE_NAME );
	WiFi.disconnect();

	WiFi.mode( WiFiMode_t::WIFI_AP );
	WiFi.softAPConfig( apIP, apIP, apMASK );
	bool res = WiFi.softAP( DEVICE_NAME, "1234567890" );

	delay( 600 );

	if( res ){
		dnsServer.setErrorReplyCode( DNSReplyCode::NoError );
		dnsServer.start( 53, "*", apIP );
	}
}

//-------------------------------------------------------------------------------
void wifi_sniffer_init(void)
{
	wifi_station_disconnect();
	wifi_set_opmode( STATION_MODE );
	wifi_set_channel( wifi_channel );
	wifi_promiscuous_enable( false );
	wifi_set_promiscuous_rx_cb( wifi_rx_cb );
	wifi_promiscuous_enable( true );
	flags.sniffing = 1;
}

//-------------------------------------------------------------------------------
void wifi_rx_cb(uint8_t *buf, uint16_t len)
{
	if (len == sizeof(struct sniffer_buf2)) {
		struct sniffer_buf2 *sniffer = (struct sniffer_buf2*) buf;
		wifi_parsing_data( sniffer->buf, sniffer->rx_ctrl.rssi, len );
	} else if (len == sizeof(struct RxControl)) {
		struct RxControl *sniffer = (struct RxControl*) buf;
	} else {
		struct sniffer_buf *sniffer = (struct sniffer_buf*) buf;
		wifi_parsing_data( sniffer->buf, sniffer->rx_ctrl.rssi, len );
	}
}

//-------------------------------------------------------------------------------
void wifi_parsing_data(uint8_t *frame, signed rssi, uint16_t len)
{
	struct wifi_frame_info info;

	memcpy( info.receiveraddr, default_mac, WIFI_MAC_LEN );
	memcpy( info.destinationaddr, default_mac, WIFI_MAC_LEN );
	memcpy( info.transmitteraddr, default_mac, WIFI_MAC_LEN );
	memcpy( info.sourceaddr, default_mac, WIFI_MAC_LEN );
	memcpy( info.bssid, default_mac, WIFI_MAC_LEN );
	info.ssid_length					= 0;
	info.isvalid						= true;
	info.channel						= 0;
	info.seq_num						= 0;
	info.raw_length						= len;
	info.frametype						= (frame[0] & B00001100) >> 2;
	info.framesubtype					= (frame[0] & B11110000) >> 4;
	info.rssi							= rssi;


	memcpy( info.raw, frame, len - 1 );
	uint8_t dstofrom = (frame[1] & 3);


	uint8_t pos;
	uint8_t parsecount;

	switch( info.frametype ){
		case WIFI_CONTROL:
			switch( info.framesubtype ){
				case WIFI_CONTROL_RTS: // - RTS
					memcpy(info.receiveraddr, frame + 4, WIFI_MAC_LEN);
					memcpy(info.destinationaddr, frame + 4, WIFI_MAC_LEN); //?
					memcpy(info.transmitteraddr, frame + 4 + WIFI_MAC_LEN, WIFI_MAC_LEN);
					memcpy(info.sourceaddr, frame + 4 + WIFI_MAC_LEN, WIFI_MAC_LEN); //?
				break;
				case WIFI_CONTROL_CTS: // - CTS
					memcpy(info.receiveraddr, frame + 4, WIFI_MAC_LEN);
					memcpy(info.destinationaddr, frame + 4, WIFI_MAC_LEN); //?
				break;
				case WIFI_CONTROL_ACK: // - ACK
					memcpy(info.receiveraddr, frame + 4, WIFI_MAC_LEN);
					memcpy(info.destinationaddr, frame + 4, WIFI_MAC_LEN); //?
				break;
				case WIFI_CONTROL_PS_POLL: // - PS-POLL
					memcpy(info.receiveraddr, frame + 4, WIFI_MAC_LEN);
					memcpy(info.destinationaddr, frame + 4, WIFI_MAC_LEN); //?
					memcpy(info.bssid, frame + 4, WIFI_MAC_LEN);
					memcpy(info.transmitteraddr, frame + 4 + WIFI_MAC_LEN, WIFI_MAC_LEN);
					memcpy(info.sourceaddr, frame + 4 + WIFI_MAC_LEN, WIFI_MAC_LEN); //?
				break;
				case WIFI_CONTROL_CF_END: // - CF-END
					memcpy(info.receiveraddr, frame + 4, WIFI_MAC_LEN);
					memcpy(info.destinationaddr, frame + 4, WIFI_MAC_LEN);
					memcpy(info.transmitteraddr, frame + 4 + WIFI_MAC_LEN, WIFI_MAC_LEN);
					memcpy(info.sourceaddr, frame + 4 + WIFI_MAC_LEN, WIFI_MAC_LEN);          
					memcpy(info.bssid, frame + 4 + WIFI_MAC_LEN, WIFI_MAC_LEN);
				break;
				case WIFI_CONTROL_CF_END_CF_ACK: // - CF-END+CF-ACK
					memcpy(info.receiveraddr, frame + 4, WIFI_MAC_LEN);
					memcpy(info.destinationaddr, frame + 4, WIFI_MAC_LEN);
					memcpy(info.transmitteraddr, frame + 4 + WIFI_MAC_LEN, WIFI_MAC_LEN);
					memcpy(info.sourceaddr, frame + 4 + WIFI_MAC_LEN, WIFI_MAC_LEN);          
					memcpy(info.bssid, frame + 4 + WIFI_MAC_LEN, WIFI_MAC_LEN);
				break;
				case WIFI_CONTROL_BLOCK_ACK_REQUEST: // - BlockAckReq
					memcpy(info.receiveraddr, frame + 4, WIFI_MAC_LEN);
					memcpy(info.destinationaddr, frame + 4, WIFI_MAC_LEN);
					memcpy(info.transmitteraddr, frame + 4 + WIFI_MAC_LEN, WIFI_MAC_LEN);
					memcpy(info.sourceaddr, frame + 4 + WIFI_MAC_LEN, WIFI_MAC_LEN);
				break;
				case WIFI_CONTROL_BLOCK_ACK: // - BlockAck
					memcpy(info.receiveraddr, frame + 4, WIFI_MAC_LEN);
					memcpy(info.destinationaddr, frame + 4, WIFI_MAC_LEN);
					memcpy(info.transmitteraddr, frame + 4 + WIFI_MAC_LEN, WIFI_MAC_LEN);
					memcpy(info.sourceaddr, frame + 4 + WIFI_MAC_LEN, WIFI_MAC_LEN);
				break;
				case WIFI_CONTROL_CONTROL_WRAPPER: // - Control Wrapper
					//TODO
				break;
        }
		break;
		case WIFI_DATA:
			info.seq_num = frame[23] * 0xFF + (frame[22] & 0xF0);
			switch( dstofrom ){ // - ToDS FromDS
				case WIFI_DS_NO: // - ToDS=0 FromDS=0
					memcpy(info.receiveraddr, frame + 4, WIFI_MAC_LEN);
					memcpy(info.destinationaddr, frame + 4, WIFI_MAC_LEN);
					memcpy(info.transmitteraddr, frame + 4 + WIFI_MAC_LEN, WIFI_MAC_LEN);
					memcpy(info.sourceaddr, frame + 4 + WIFI_MAC_LEN, WIFI_MAC_LEN);
					memcpy(info.bssid, frame + 4 + WIFI_MAC_LEN * 2, WIFI_MAC_LEN);
				break;
				case WIFI_DS_TO: // - ToDS=1 FromDS=0
					memcpy(info.receiveraddr, frame + 4, WIFI_MAC_LEN);
					memcpy(info.bssid, frame + 4, WIFI_MAC_LEN);
					memcpy(info.transmitteraddr, frame + 4 + WIFI_MAC_LEN, WIFI_MAC_LEN);
					memcpy(info.sourceaddr, frame + 4 + WIFI_MAC_LEN, WIFI_MAC_LEN);
					memcpy(info.destinationaddr, frame + 4 + WIFI_MAC_LEN * 2, WIFI_MAC_LEN); //MSDU
				break;
				case WIFI_DS_FROM: // - ToDS=0 FromDS=1
					memcpy(info.receiveraddr, frame + 4, WIFI_MAC_LEN);
					memcpy(info.destinationaddr, frame + 4, WIFI_MAC_LEN);
					memcpy(info.transmitteraddr, frame + 4 + WIFI_MAC_LEN, WIFI_MAC_LEN);
					memcpy(info.bssid, frame + 4 + WIFI_MAC_LEN, WIFI_MAC_LEN);
					memcpy(info.sourceaddr, frame + 4 + WIFI_MAC_LEN * 2, WIFI_MAC_LEN);
				break;
				case WIFI_DS_TOFROM: // - ToDS=1 FromDS=1
					memcpy(info.receiveraddr, frame + 4, WIFI_MAC_LEN);
					memcpy(info.transmitteraddr, frame + 4 + WIFI_MAC_LEN, WIFI_MAC_LEN);
					memcpy(info.destinationaddr, frame + 4 + WIFI_MAC_LEN * 2, WIFI_MAC_LEN);
					memcpy(info.sourceaddr, frame + 4 + WIFI_MAC_LEN * 2 + 2, WIFI_MAC_LEN);
				break;
			}
		break;
		case WIFI_MANAGEMENT:
			switch( info.framesubtype ){
				case WIFI_MANAGEMENT_PROBE_RESPONSE:
				case WIFI_MANAGEMENT_BEACON:
					info.seq_num		= frame[23] * 0xFF + (frame[22] & 0xF0);
					pos					= WIFI_MANAGEMENT_MAC_HEADER_SIZE;

					// FIXME: Говнокод!!!!
					while (pos < len && parsecount < 4) {
						switch (frame[pos]) {
							case 0: // - SSID
								info.ssid_length = frame[pos + 1];
								if (info.ssid_length > 32 || info.ssid_length < 0) {
									info.ssid_length = 0;
								}
								memcpy(info.ssid, frame + pos + 2, info.ssid_length);
								info.ssid[ info.ssid_length ] = '\0';
							break;
							case 3: // - Channel
								info.channel = (int) frame[pos + 2];
							break;
							default: break;
						}
						parsecount++; // - Avoid bad parsing or infinite loop
						pos += frame[pos + 1] + 2;
					}
				break;
			}

			memcpy( info.receiveraddr, frame + 4, WIFI_MAC_LEN );
			memcpy( info.destinationaddr, frame + 4, WIFI_MAC_LEN );
			memcpy( info.transmitteraddr, frame + 4 + WIFI_MAC_LEN, WIFI_MAC_LEN );
			memcpy( info.sourceaddr, frame + 4 + WIFI_MAC_LEN, WIFI_MAC_LEN );
			memcpy( info.bssid, frame + 4 + WIFI_MAC_LEN * 2, WIFI_MAC_LEN );
		break;
	}

	if( info.bssid[ 0 ] == 0 && info.bssid[ 1 ] == 0 && info.bssid[ 2 ] == 0 && info.bssid[ 3 ] == 0 && info.bssid[ 4 ] == 0 && info.bssid[ 5 ] == 0 ) return;

	Serial.print( "BSSID: " );
	for( uint8_t i = 0; i < sizeof( info.bssid ); i++ ){
		if( i > 0 ) Serial.print( ":" );
		if( info.bssid[ i ] < 16 ) Serial.print( "0" );
		Serial.print( info.bssid[ i ], HEX );
	}
			// Serial.print( "	SRC: " );
			// for( uint8_t i = 0; i < sizeof( info.sourceaddr ); i++ ){
			// 	if( i > 0 ) Serial.print( ":" );
			// 	if( info.sourceaddr[ i ] < 16 ) Serial.print( "0" );
			// 	Serial.print( info.sourceaddr[ i ], HEX );
			// }
			// Serial.print( "	RCV: " );
			// for( uint8_t i = 0; i < sizeof( info.receiveraddr ); i++ ){
			// 	if( i > 0 ) Serial.print( ":" );
			// 	if( info.receiveraddr[ i ] < 16 ) Serial.print( "0" );
			// 	Serial.print( info.receiveraddr[ i ], HEX );
			// }
			//Serial.print( "	" ); Serial.println( (char*)info.ssid );
	

	if( maccmp( info.sourceaddr, friendmac ) || maccmp( info.receiveraddr, friendmac )) { 
		Serial.print( "	>>> is here! >>>	" );
		unlockCounter = 15;
	}else if( maccmp( info.sourceaddr, friendmac2 ) || maccmp( info.receiveraddr, friendmac2 )) {
		Serial.print( "	>>> is here! >>>	" );
		unlockCounter = 15;
	}

	Serial.println();
}

//-------------------------------------------------------------------------------
bool maccmp(uint8_t *mac1, uint8_t *mac2)
{
	for( uint8_t i = 0; i < WIFI_MAC_LEN; i++ ){ 
		if( mac1[ i ] != mac2[ i ] ){ 
			return false; 
		} 
	} 
	return true; 
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------


