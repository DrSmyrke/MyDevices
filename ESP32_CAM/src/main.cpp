#include "main.h"
#include "esp_camera.h"
#include <WiFi.h>
#include <WiFiUdp.h>


// #include <arduino_homekit_server.h>
// #include <homekit/homekit.h>
// #include <homekit/characteristics.h>


// #include "homekit_main.h"


//
// WARNING!!! PSRAM IC required for UXGA resolution and high JPEG quality
//            Ensure ESP32 Wrover Module or other board with PSRAM is selected
//            Partial images will be transmitted if image exceeds buffer size
//

// Select camera model
// #define CAMERA_MODEL_WROVER_KIT // Has PSRAM
//#define CAMERA_MODEL_ESP_EYE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_PSRAM // Has PSRAM
//#define CAMERA_MODEL_M5STACK_V2_PSRAM // M5Camera version B Has PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_ESP32CAM // No PSRAM
#define CAMERA_MODEL_AI_THINKER // Has PSRAM
//#define CAMERA_MODEL_TTGO_T_JOURNAL // No PSRAM

#include "camera_pins.h"

void startCameraServer();


//------------------- VARIABLES ---------------------------------------------------------
WebServer webServer( 8080 );
char pageBuff[ WEB_PAGE_BUFF_SIZE ];
uint8_t updateFlag;
TaskHandle_t Task1;

// extern "C" homekit_server_config_t config;
// extern "C" homekit_characteristic_t outlet_on;
// extern "C" homekit_characteristic_t outlet_in_use;

//---------------------------------------------------------------------------------------
void webServerTaskCode(void *parameter)
{
	for( ;; ){
		webServer.handleClient();
	}
}

//---------------------------------------------------------------------------------------
void setup()
{
	esp::init();

	updateFlag				= 0;


	// pinMode( FLASH_LED_PIN, OUTPUT );
	pinMode( LED_PIN, OUTPUT );
	digitalWrite( LED_PIN, HIGH );
	// ledcSetup( 0, 5000, 8 );
	// ledcAttachPin( FLASH_LED_PIN, 0 );
	// ledcWrite( 0, 255 );

	

	camera_config_t config;
	config.ledc_channel = LEDC_CHANNEL_0;
	config.ledc_timer = LEDC_TIMER_0;
	config.pin_d0 = Y2_GPIO_NUM;
	config.pin_d1 = Y3_GPIO_NUM;
	config.pin_d2 = Y4_GPIO_NUM;
	config.pin_d3 = Y5_GPIO_NUM;
	config.pin_d4 = Y6_GPIO_NUM;
	config.pin_d5 = Y7_GPIO_NUM;
	config.pin_d6 = Y8_GPIO_NUM;
	config.pin_d7 = Y9_GPIO_NUM;
	config.pin_xclk = XCLK_GPIO_NUM;
	config.pin_pclk = PCLK_GPIO_NUM;
	config.pin_vsync = VSYNC_GPIO_NUM;
	config.pin_href = HREF_GPIO_NUM;
	config.pin_sscb_sda = SIOD_GPIO_NUM;
	config.pin_sscb_scl = SIOC_GPIO_NUM;
	config.pin_pwdn = PWDN_GPIO_NUM;
	config.pin_reset = RESET_GPIO_NUM;
	config.xclk_freq_hz = 20000000;
	config.pixel_format = PIXFORMAT_JPEG;
	
	// if PSRAM IC present, init with UXGA resolution and higher JPEG quality
	//                      for larger pre-allocated frame buffer.
	if(psramFound()){
		config.frame_size = FRAMESIZE_UXGA;
		config.jpeg_quality = 10;
		config.fb_count = 2;
	} else {
		config.frame_size = FRAMESIZE_SVGA;
		config.jpeg_quality = 12;
		config.fb_count = 1;
	}

#if defined(CAMERA_MODEL_ESP_EYE)
	pinMode(13, INPUT_PULLUP);
	pinMode(14, INPUT_PULLUP);
#endif

	// camera init
	esp_err_t err = esp_camera_init(&config);
	if (err != ESP_OK) {
		Serial.printf("Camera init failed with error 0x%x", err);
		return;
	}

	sensor_t * s = esp_camera_sensor_get();
	// initial sensors are flipped vertically and colors are a bit saturated
	if (s->id.PID == OV3660_PID) {
		s->set_vflip(s, 1); // flip it back
		s->set_brightness(s, 1); // up the brightness just a bit
		s->set_saturation(s, -2); // lower the saturation
	}
	// drop down frame size for higher initial frame rate
	s->set_framesize(s, FRAMESIZE_QVGA);

#if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
	s->set_vflip(s, 1);
	s->set_hmirror(s, 1);
#endif

	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
	}

	startCameraServer();


	esp::pageBuff = pageBuff;
	esp::addWebServerPages( &webServer, true, true, true );
	webServer.on( "/", handleRoot );
	webServer.on( "/get", HTTP_GET, handleGet );
	webServer.on( ESP_FIRMWARE_FILENAME, HTTP_GET, [ webServer ](void){
		esp::webSendFile( &webServer, ESP_FIRMWARE_FILENAME, "application/binary" );
	} );
	webServer.on( "/update", HTTP_ANY, handleUpdate );
	webServer.on( "/upload", HTTP_POST, [](){}, handleUpload );
	webServer.begin();

	xTaskCreatePinnedToCore( webServerTaskCode, "Task1", 10000, NULL, 0, &Task1, 0 );


	digitalWrite( LED_PIN, LOW );
	// ledcWrite( 0, 0 );

}

//---------------------------------------------------------------------------------------
void loop()
{
	// if( updateFlag ){
	// 	updateFlag = 0;
	// 	if( esp::updateFromFS() ) ESP.restart();
	// }
}
