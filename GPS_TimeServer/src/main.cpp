//------------------------------------------------------
#define DEV_NAME				"GPS_TIMESERVER"
#define WIFI_NETWORK			""
#define WIFI_NETWORK_KEY		""
//------------------------------------------------------
#include <EEPROM.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <iarduino_GPS_NMEA.h>
//------------------------------------------------------
ESP8266WebServer webServer(80);
iarduino_GPS_NMEA gps;
char* sa[] = { "NoName", "GPS", "Глонасс", "Galileo", "Beidou", "QZSS" };
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
	
	webServer.on("/", httpHandleIndex);
	webServer.on("/data", httpHandleData);
	//webServer.on("/index.css", httpHandleCssFile);
	webServer.onNotFound(httpHandleNotFound);
	webServer.begin();
	
	delay(100);
	timer_init();
	gps.begin( Serial );
	gps.timeOut( 500 );
	gps.timeZone( GPS_AutoDetectZone );
}

void loop()
{
	webServer.handleClient();

	gps.read();
	//if( gps.errPos ){

	//}
	
	//while( Serial.available() ) {
		//char sym = (char)Serial.read();
		//conf.rxBuff[ conf.wIndx++ & UART_BUFFER_MASK] = sym;
    //}
}
