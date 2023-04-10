#include "http.h"
#include "main.h"
#include <Update.h>

//------------------- STRUCTIRES --------------------------------------------------------
enum{
	upload_type_na,
	upload_type_file,
	upload_type_firmware,
	upload_state_start,
	upload_state_process,
	upload_state_end,
};

//------------------- VARIABLES ---------------------------------------------------------
uint8_t uploadType				= upload_type_na;
uint8_t uploadState				= upload_state_end;
File fsUploadFile;

//-------------------------------------------------------------------------------
void handleRoot(void)
{
	esp::webSendFile( &webServer, "/index.html", "text/html" );
}

//-------------------------------------------------------------------------------
void handleGet(void)
{
	esp::setNoCacheContent( &webServer );

	strcpy( pageBuff, "{" );
	//-------------------------------------------------------------
	strcat( pageBuff, "\"version\": " ); strcat( pageBuff, FIRMWARE_SECOND_VERSION );
	//-------------------------------------------------------------
	strcat( pageBuff, "}" );
	webServer.send ( 200, "application/json", pageBuff );
}

//-------------------------------------------------------------------------------
void handleUpdate(void)
{
	esp::setNoCacheContent( &webServer );
	static String test = "";
	//-------------------------------------------------------------
	// if( !esp::checkWebAuth( &webServer, ADMIN_LOGIN, ADMIN_PASSWORD, "Dr.Smyrke access zone", "Access denied :(" ) ) return;
	//-------------------------------------------------------------
	if( webServer.hasArg( "cmd" ) ){
		if( webServer.arg( "cmd" ) == "update" ){
			HTTPUpload& upload = webServer.upload();

			String filename = "/" + upload.filename;
			if(!filename.startsWith("/")) filename = "/"+filename;

			if( upload.filename == "index.html" || upload.filename == "index.js" || upload.filename == "index.css" ){
				uploadType = upload_type_file;
			}else if( upload.filename == "firmware.bin" ){
				uploadType = upload_type_firmware;
			}
			
			if( uploadState == upload_state_end ){
				ESP_DEBUG( "Update: %s\n", upload.filename.c_str() );
				if( uploadType == upload_type_file ){
#if defined(ARDUINO_ARCH_ESP8266)
					LittleFS.remove( filename );
					fsUploadFile = LittleFS.open( filename, "w");
#elif defined(ARDUINO_ARCH_ESP32)
					SPIFFS.remove( filename );
					fsUploadFile = SPIFFS.open( filename, "w");
					test = filename + String( ":" ) + String( fsUploadFile );
#endif
				}else if( uploadType == upload_type_firmware ){
					if( !Update.begin( UPDATE_SIZE_UNKNOWN ) ) Update.printError( Serial );
				}
				uploadState = upload_state_start;
			}

			if( upload.currentSize ) writeProcess( upload.buf, upload.currentSize );

			if( upload.status == UPLOAD_FILE_END ){
				if( uploadType == upload_type_file ){
					if( fsUploadFile ){
						fsUploadFile.close();
						webServer.send ( 200, "text/plain", "OK" );
					}else{
						webServer.send ( 400, "text/plain", "ERROR" );
					}
					webServer.send ( 200, "text/plain", "OK" );
				}else if( uploadType == upload_type_firmware ){
					if( Update.end() ){
						ESP_DEBUG( "Update Success: %u\nRebooting...\n", upload.totalSize );
						webServer.send( 200, "text/plain", ( Update.hasError() ) ? "FAIL" : "OK" );
						delay( 1000 );
						ESP.restart();
					}else{
						Update.printError( Serial );
						webServer.send( 401, "text/plain", Update.errorString() );
					}
				}
				uploadState = upload_state_end;
			}
			return;
		}else if( webServer.arg( "cmd" ) == "check_update" ){
			uint32_t version = atoi( FIRMWARE_SECOND_VERSION );
			strcpy( pageBuff, "{" );
				strcat( pageBuff, "\"newVersion\": " ); utoa( esp::checkingUpdate( FIRMWARE_REPOSITORY, version ), esp::tmpVal, 10 ); strcat( pageBuff, esp::tmpVal );
			strcat( pageBuff, "}" );

			webServer.send ( 200, "application/json", pageBuff );
			return;
		}
	}
	//-------------------------------------------------------------

	webServer.send ( 200, "text/html", "" );

// 	/*handling uploading firmware file */
//   server.on("/update", HTTP_POST, []() {
//     server.sendHeader("Connection", "close");
//     
//     ESP.restart();
//   }, []() {
//     HTTPUpload& upload = server.upload();
    
//   });
}

//-------------------------------------------------------------------------------
void writeProcess(uint8_t *data, const size_t size)
{
	if( uploadType == upload_type_file ){
		if( fsUploadFile ) fsUploadFile.write( data, size );
	}else if( uploadType == upload_type_firmware ){
		if( Update.write( data, size ) != size ) Update.printError( Serial );
	}
	uploadState = upload_state_process;
}

//-------------------------------------------------------------------------------
void handleUpload(void)
{
	/*
	HTTPUpload& upload = webServer.upload();
	String filename;
	
	switch( upload.status ){
		case UPLOAD_FILE_START:
			filename = upload.filename;
			if(!filename.startsWith("/")) filename = "/"+filename;

			if( filename == CONFFILE ){
				uploadType = upload_type_file;
				fsUploadFile = SPIFFS.open(CONFFILE, "w");
			}
			if( upload.filename == UPDFILE ){
				WiFiUDP::stopAll();
				uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
				uploadType = upload_type_firmware;
				Update.begin(maxSketchSpace);
			}
		break;
		case UPLOAD_FILE_WRITE:
			if( uploadType == upload_type_file && fsUploadFile ) fsUploadFile.write(upload.buf,upload.currentSize);
			//if( fsUploadFile) fsUploadFile.write(upload.buf,upload.currentSize);
			//if(Update.write(upload.buf, upload.currentSize) != upload.currentSize)
			if( uploadType == upload_type_firmware) Update.write(upload.buf, upload.currentSize);
		break;
		case UPLOAD_FILE_END:
			if( uploadType == upload_type_file && fsUploadFile ){
				fsUploadFile.close();
				delay(500);
				webServer.sendHeader("Location","/config");
				webServer.send(303);
			}
			if( uploadType == upload_type_firmware){
				if( Update.end(true) ){
					//webServer.sendHeader("Location","/config");
					//webServer.send(303);
					//delay(500);
					ESP.restart();
				}else{
					webServer.send(500, "text/html", "Upload error" );
				}
			}
		break;
	}
	*/
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
