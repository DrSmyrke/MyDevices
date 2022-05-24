#ifndef _MAIN_H_
#define _MAIN_H_


//-------------------------------------------------------------------------------
#define DEVICE_NAME							"DNS_Secondary"
#define WEB_PAGE_BUFF_SIZE					2048
#define RULES_FILE							"/rules"
#define MAX_DNS_RECORDS						128

//-------------------------------------------------------------------------------
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <DNS_Server.h>
#include <esp_functions.h>


//-------------------------------------------------------------------------------
typedef struct{
	// unsigned char timer						: 1;
	// unsigned char apMode					: 1;
	// unsigned char captivePortalAccess		: 1;
} MainFlags;

typedef struct{
	// unsigned char timer						: 1;
	// unsigned char apMode					: 1;
	// unsigned char captivePortalAccess		: 1;
} DNS_Rule;

//-------------------------------------------------------------------------------
extern DNS_Server dnsServer;
extern ESP8266WebServer webServer;
extern MainFlags flags;
extern char tmpVal[ 10 ];
extern char pageBuff[ WEB_PAGE_BUFF_SIZE ];

//-------------------------------------------------------------------------------






#endif //_MAIN_H_
