#ifndef _MAIN_H_
#define _MAIN_H_


//-------------------------------------------------------------------------------
#define DEVICE_NAME							"GSM_HUB"
#define GSM_RESET_PIN						D7
#define GSM_PORT							gsmSerial
#define GSM_BEGIN							GSM_PORT.begin( 115200 )
#define WEB_PAGE_BUFF_SIZE					2048
#define GSM_DATA_BUFF_SIZE					256

#define GSM_SMS_INIT						"AT+CMGF=1"
#define GSM_CONTYPE_INIT					"AT+SAPBR=3,1,CONTYPE,\"GPRS\""
#define GSM_APN_INIT						"AT+SAPBR=3,1,APN,\"internet.tele2.ru\""
#define GSM_USER_INIT						"AT+SAPBR=3,1,USER,\"\""
#define GSM_PWD_INIT						"AT+SAPBR=3,1,PWD,\"\""

//-------------------------------------------------------------------------------
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <DNS_Server.h>
#include <esp_functions.h>
#include <AT.h>


//-------------------------------------------------------------------------------
typedef struct {
	int8_t rssi;
	uint8_t ber;
	uint8_t reg;
	char opLabel[ 32 ];
} GSM_Data;

//-------------------------------------------------------------------------------
extern SoftwareSerial GSM_PORT;
extern DNS_Server dnsServer;
extern ESP8266WebServer webServer;
extern AT at;
extern GSM_Data gsmData;
extern char tmpVal[ 10 ];
extern char pageBuff[ WEB_PAGE_BUFF_SIZE ];
extern char gsm_data_buff[ GSM_DATA_BUFF_SIZE ];

//-------------------------------------------------------------------------------






#endif //_MAIN_H_
