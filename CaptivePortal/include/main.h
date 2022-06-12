#ifndef _MAIN_H_
#define _MAIN_H_


//-------------------------------------------------------------------------------
#define DEVICE_NAME							"CAPTIVE_PORTAL"

//-------------------------------------------------------------------------------
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
// #include <LittleFS.h>
#include <DNSServer.h>
// #include <WiFiUdp.h>
// #include <ArduinoOTA.h>


//-------------------------------------------------------------------------------
typedef struct{
	unsigned char timer						: 1;
	unsigned char apMode					: 1;
	unsigned char captivePortalAccess		: 1;
	unsigned char access					: 1;
} MainFlags;

//-------------------------------------------------------------------------------
extern DNSServer dnsServer;
extern ESP8266WebServer webServer;
extern MainFlags flags;
const extern IPAddress apIP;
const extern IPAddress apMASK;

//-------------------------------------------------------------------------------






#endif //_MAIN_H_
