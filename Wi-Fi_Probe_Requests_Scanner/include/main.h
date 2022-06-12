#ifndef _MAIN_H_
#define _MAIN_H_


//-------------------------------------------------------------------------------
#define DEVICE_NAME							"GSM_HUB"
#define WIFI_CHANNEL_SWITCH_INTERVAL		(500)
#define WIFI_CHANNEL_MAX					(13)

//-------------------------------------------------------------------------------
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <DNSServer.h>
// #include <WiFiUdp.h>
// #include <ArduinoOTA.h>


//-------------------------------------------------------------------------------
typedef struct{
	unsigned char timer						: 1;
	unsigned char apMode					: 1;
	unsigned char captivePortalAccess		: 1;
	unsigned char sniffing					: 1;
	unsigned char unlocked					: 1;
} MainFlags;

//-------------------------------------------------------------------------------
extern DNSServer dnsServer;
extern ESP8266WebServer webServer;
extern MainFlags flags;
const extern IPAddress apIP;
const extern IPAddress apMASK;
extern uint8_t wifi_channel;
extern uint8_t unlockCounter;

//-------------------------------------------------------------------------------






#endif //_MAIN_H_
