//------------------------------------------------------
#define DEV_NAME				"GPS_TIMESERVER"
#define WIFI_NETWORK			"SECRETNET"
#define WIFI_NETWORK_KEY		"##SSD3000##"
#define DEFAULT_LOGIN			"admin"
#define DEFAULT_PASS			"admin"
#define UPDFILE					"main.bin"
#define NTP_PORT				123
#define NTP_BUFF_SIZE			50
//------------------------------------------------------
#include <EEPROM.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <iarduino_GPS_NMEA.h>
//------------------------------------------------------
iarduino_GPS_NMEA gps;
//------------------------------------------------------
#include "functions.h"
#include "timer.h"
#include "timer.h"
//------------------------------------------------------
ESP8266WebServer webServer(80);
char* sa[] = { "NoName", "GPS", "Глонасс", "Galileo", "Beidou", "QZSS" };
uint8_t ntp_packet[ NTP_BUFF_SIZE ];
WiFiUDP UDP;
uint8_t ntp_packet_length = 0;
//------------------------------------------------------
struct NTP_Time{
	uint32_t second;
	uint32_t splitSecond;
};

struct NTP_Packet
{
	uint8_t ik;
	uint8_t stratum;
	uint8_t pool;
	uint8_t precision;
	uint32_t rootdelay;
	uint32_t rootdisp;
	uint32_t refID;
	NTP_Time reference;
	NTP_Time originate;
	NTP_Time recieve;
	NTP_Time transmit;
};

//------------------------------------------------------
#include "functions.h"
#include "http.h"
#include "timer.h"
//------------------------------------------------------

void setup()
{
	Serial.begin( 9600 );
	delay( 100 );
	wifi_init();

	ntp_packet_length = sizeof( NTP_Packet );
	
	webServer.on("/", httpHandleIndex);
	webServer.on("/data", httpHandleData);
	webServer.on("/config", httpHandleSettings);
	webServer.on("/upload", HTTP_POST, [](){}, handleUpload);
	webServer.onNotFound(httpHandleNotFound);
	webServer.begin();
	
	delay(100);
	timer_init();
	gps.begin( Serial );
	gps.timeOut( 500 );
	gps.timeZone( GPS_AutoDetectZone );

	UDP.begin( NTP_PORT );
}

void loop()
{
	webServer.handleClient();

	gps.read();
	
	int packetSize = UDP.parsePacket();
	if( packetSize ){
		int len = UDP.read( ntp_packet, NTP_BUFF_SIZE );
		if( len ){
			NTP_Packet* packet = ( NTP_Packet * ) ntp_packet;
			//TODO: CHECKING incomming data!!!!!!!!!!!!!!

			packet->ik = 0b00100100;
			packet->stratum = 1;
			packet->pool = 10;
			packet->precision = 0xe8;
			packet->rootdelay = 0x00001000;
			packet->rootdisp = 0xa2500000;
			packet->refID = ( 0 << 24 ) | ( 'S' << 16 ) | ( 'P' << 8 ) | ( 'G' );
			packet->reference.second = gps.Unix - 1;
			packet->reference.splitSecond = 0x52a;
			//packet->originate;
			packet->recieve.second = gps.Unix;
			packet->recieve.splitSecond = 0xe8;
			packet->transmit.second = gps.Unix;
			packet->transmit.splitSecond = packet->recieve.splitSecond + 0xe8;

			UDP.beginPacket( UDP.remoteIP(), UDP.remotePort() );
			UDP.write( ntp_packet, ntp_packet_length );
			UDP.endPacket();
		}
	}
}
