//------------------------------------------------------
bool http_auth_check(void)
{
	if( !webServer.authenticate( DEFAULT_LOGIN, DEFAULT_PASS ) ){
		webServer.requestAuthentication(DIGEST_AUTH, "DrSmyrke access", "authFail");
		return false;
	}
	return true;
}

//------------------------------------------------------
void httpHandleIndex(void)
{
	String latitude = String( gps.latitude, 5 );
	String longitude = String( gps.longitude, 5 );
	
	String page = 
	"<!DOCTYPE html><html lang=\"en\"><head lang=\"en\"><title>-= GPS =-</title></head><body>"
	"<b>latitude:</b> " + latitude + "°<br>"
	"<b>longitude:</b> " + longitude + "°<br>"
	"<b>altitude:</b> " + String( gps.altitude ) + "m<br>"
	"<b>time:</b> " + String( gps.Hours ) + ":" + String( gps.minutes ) + ":" + String( gps.seconds ) + " " +
	String( gps.day ) + "." + String( gps.month ) + "." + String( gps.year ) + "<br>"

	"<b>Unix:</b> " + String( gps.Unix ) + "<br>"
	"<b>speed:</b> " + String( gps.speed ) + "km/h<br>"
	"<b>satellites:</b> " + String( gps.satellites[ GPS_ACTIVE ] ) + "/" + String( gps.satellites[ GPS_VISIBLE ] ) + "<br>"

	"<a href=\"http://maps.yandex.ru/?ll=" + longitude + "," + latitude + "&pt=" + longitude + "," + latitude + "&l=map&z=18\" target=\"_blank\">MAP</a>"

	"</body></html>"
	;

	webServer.send(200, "text/html", page);
}

//------------------------------------------------------
void httpHandleData(void)
{
	String page = 
	"{"
	"\"latitude\":  \"" + String( gps.latitude, 5 ) + "\","
	"\"longitude\":  \"" + String( gps.longitude, 5 ) + "\","
	"\"altitude\":  \"" + String( gps.altitude ) + "\","
	"\"hour\":  \"" + String( gps.Hours ) + "\","
	"\"minutes\":  \"" + String( gps.minutes ) + "\","
	"\"seconds\":  \"" + String( gps.seconds ) + "\","
	"\"day\":  \"" + String( gps.day ) + "\","
	"\"month\":  \"" + String( gps.month ) + "\","
	"\"year\":  \"" + String( gps.year ) + "\","
	"\"Unix\":  \"" + String( gps.Unix ) + "\","
	"\"speed\":  \"" + String( gps.speed ) + "\","
	"\"satellites\":  \"" + String( gps.satellites[ GPS_ACTIVE ] ) + "/" + String( gps.satellites[ GPS_VISIBLE ] ) + "\""

	//http://maps.yandex.ru/?ll=gps.longitude,gps.latitude&pt=gps.longitude,gps.latitude&l=map&z=18
	"}"
	;

	webServer.send(200, "application/json", page);
}

//------------------------------------------------------
void httpHandleSettings(void)
{
	if( !http_auth_check() ) return;
	
	String page = "<form action=\"/upload\" method=\"POST\" enctype=\"multipart/form-data\"><center class=\"valfiol\">Upload assets</center><br><input type=\"file\" name=\"file\"> <input type=\"submit\" value=\"Upload\"></form>";

	webServer.send(200, "text/html", page);
}

//------------------------------------------------------
void handleUpload(void)
{
	if( !http_auth_check() ) return;

	HTTPUpload& upload = webServer.upload();
	String filename;
	
	switch( upload.status ){
		case UPLOAD_FILE_START:
			filename = upload.filename;
			if(!filename.startsWith("/")) filename = "/"+filename;

			// if( filename == CONFFILE ){
			// 	conf.uploadType = upload_type_file;
			// 	fsUploadFile = SPIFFS.open(CONFFILE, "w");
			// }
			if( upload.filename == UPDFILE ){
				WiFiUDP::stopAll();
				uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
				// conf.uploadType = upload_type_firmware;
				Update.begin(maxSketchSpace);
			}
		break;
		case UPLOAD_FILE_WRITE:
			// if( conf.uploadType == upload_type_file && fsUploadFile ) fsUploadFile.write(upload.buf,upload.currentSize);
			
			//if( fsUploadFile) fsUploadFile.write(upload.buf,upload.currentSize);
			//if(Update.write(upload.buf, upload.currentSize) != upload.currentSize)
			if( upload.filename == UPDFILE ) Update.write(upload.buf, upload.currentSize);
		break;
		case UPLOAD_FILE_END:
			// if( conf.uploadType == upload_type_file && fsUploadFile ){
			// 	fsUploadFile.close();
			// 	delay(500);
			// 	webServer.sendHeader("Location","/config");
			// 	webServer.send(303);
			// }
			if( upload.filename == UPDFILE ){
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
}

//------------------------------------------------------
void httpHandleNotFound(void)
{
	webServer.send( 404, "text/html", "" );
}
