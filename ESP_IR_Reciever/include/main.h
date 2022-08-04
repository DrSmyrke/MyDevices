#ifndef _MAIN_H_
#define _MAIN_H_


//-------------------------------------------------------------------------------
#define DEVICE_NAME							"IR Reciever"
#define PUT_URL						    	"http://api.webserver.lan/MySmartHoBot/sendMessage"
#define KEY						    	    "0f7d848c56094bf722ca34701d38938e"
#define WEB_PAGE_BUFF_SIZE					2048

//-------------------------------------------------------------------------------
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <DNS_Server.h>
#include <esp_functions.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>

//-------------------------------------------------------------------------------


//-------------------------------------------------------------------------------
extern DNS_Server dnsServer;
extern ESP8266WebServer webServer;
// extern MainFlags flags;
extern char pageBuff[ WEB_PAGE_BUFF_SIZE ];

//-------------------------------------------------------------------------------






#endif //_MAIN_H_
