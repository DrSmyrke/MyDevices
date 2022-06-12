#ifndef _MAIN_H_
#define _MAIN_H_


//-------------------------------------------------------------------------------
#define DEVICE_NAME							"GSM_HUB"
#define GSM_RESET_PIN						14
#define GSM_BEGIN							Serial.begin(115200)
#define GSM_AVAILABLE						Serial.available()
#define GSM_READ							Serial.read()

#define GSM_SMS_INIT						"AT+CMGF=1"
#define GSM_CONTYPE_INIT					"AT+SAPBR=3,1,CONTYPE,\"GPRS\""
#define GSM_APN_INIT						"AT+SAPBR=3,1,APN,\"internet.tele2.ru\""
#define GSM_USER_INIT						"AT+SAPBR=3,1,USER,\"\""
#define GSM_PWD_INIT						"AT+SAPBR=3,1,PWD,\"\""

//-------------------------------------------------------------------------------
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <DNSServer.h>
#include <esp_functions.h>
#include <AT.h>
// #include <WiFiUdp.h>
// #include <ArduinoOTA.h>


//-------------------------------------------------------------------------------
typedef struct{
	unsigned char timer						: 1;
	unsigned char apMode					: 1;
	unsigned char captivePortalAccess		: 1;
} MainFlags;

typedef struct {
	int8_t rssi;
	uint8_t ber;
	uint8_t reg;
	char opLabel[ 32 ];
} GSM_Data;

//-------------------------------------------------------------------------------
extern DNSServer dnsServer;
extern ESP8266WebServer webServer;
extern MainFlags flags;
const extern IPAddress apIP;
const extern IPAddress apMASK;
extern int8_t countNetworks;
extern long wtimer;
extern ATCommand AT;
extern GSM_Data gsmData;
extern char tmpVal[ 10 ];
extern char gsm_data_buff[ 512 ];

//-------------------------------------------------------------------------------






#endif //_MAIN_H_
