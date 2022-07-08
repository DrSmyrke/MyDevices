#include "functions.h"
#include "main.h"

//-------------------------------------------------------------------------------
void updateDevice(void)
{
	esp::downloadUpdate( FIRMWARE_REPOSITORY, "/index.html" );
	esp::downloadUpdate( FIRMWARE_REPOSITORY, "/index.css" );
	esp::downloadUpdate( FIRMWARE_REPOSITORY, "/index.js" );
	if( esp::downloadUpdate( FIRMWARE_REPOSITORY, ESP_FIRMWARE_FILENAME ) ) ;//updateFlag = 1;
}

//-------------------------------------------------------------------------------


//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
// HTTPUpload& upload = webServer.upload();
			// String filename;
			// switch( upload.status ){
			// 	case UPLOAD_FILE_START:
			// 		filename = upload.filename;
			// 		if(!filename.startsWith("/")) filename = "/"+filename;

			// 		if( upload.filename == FIRMWARE_FILE ){
			// 			// WiFiUDP::stopAll();
			// 			uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
			// 			uploadType = upload_type_firmware;
			// 			Update.begin(maxSketchSpace);
			// 		}else{
			// 			uploadType = upload_type_file;
			// 			fsUploadFile = SPIFFS.open( filename, "w");
			// 		}
			// 	break;
			// 	case UPLOAD_FILE_WRITE:
			// 		if( uploadType == upload_type_file && fsUploadFile ) fsUploadFile.write(upload.buf,upload.currentSize);
			// 		//if( fsUploadFile) fsUploadFile.write(upload.buf,upload.currentSize);
			// 		//if(Update.write(upload.buf, upload.currentSize) != upload.currentSize)
			// 		if( uploadType == upload_type_firmware) Update.write(upload.buf, upload.currentSize);
			// 	break;
			// 	case UPLOAD_FILE_END:
			// 		if( uploadType == upload_type_file && fsUploadFile ){
			// 			fsUploadFile.close();
			// 			delay(500);
			// 			webServer.sendHeader( "Location", "/" );
			// 			webServer.send( 303 );
			// 		}else if( uploadType == upload_type_firmware){
			// 			if( Update.end(true) ){
			// 				//webServer.sendHeader("Location","/config");
			// 				//webServer.send(303);
			// 				//delay(500);
			// 				ESP.restart();
			// 			}else{
			// 				webServer.send( 500, "text/html", "Upload error" );
			// 			}
			// 		}
			// 	break;
			// }

