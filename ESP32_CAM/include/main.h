#ifndef __MAIN_H_
#define __MAIN_H_


//-------------- DEFINES ------------------------------------------------
#define LED_PIN											GPIO_NUM_33
#define FLASH_LED_PIN									GPIO_NUM_4
#define FLASH_LED_PWM_CHANNEL							0
#define DEVICE_NAME										"ESP32_HOME_CAM"
#define WEB_PAGE_BUFF_SIZE								2048
#define FIRMWARE_REPOSITORY								"http://firmware.drsmyrke.keenetic.pro/esp32_cam"
#define ADMIN_LOGIN										"admin"
#define ADMIN_PASSWORD									"admin"

// Select camera model
// #define CAMERA_MODEL_WROVER_KIT // Has PSRAM
// #define CAMERA_MODEL_ESP_EYE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_PSRAM // Has PSRAM
//#define CAMERA_MODEL_M5STACK_V2_PSRAM // M5Camera version B Has PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_ESP32CAM // No PSRAM
#define CAMERA_MODEL_AI_THINKER // Has PSRAM
//#define CAMERA_MODEL_TTGO_T_JOURNAL // No PSRAM

//-------------- INCLUDES -----------------------------------------------
#include "camera_pins.h"
#include <Arduino.h>
#include <esp_functions.h>
#include <SPIFFS.h>
#include "http.h"
#include "functions.h"

//------------------- VARIABLES ---------------------------
extern WebServer webServer;
extern char tmpVal[ 12 ];
extern char pageBuff[ WEB_PAGE_BUFF_SIZE ];
extern uint8_t updateFlag;


#endif //__MAIN_H_