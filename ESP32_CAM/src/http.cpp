#include "http.h"
#include "main.h"

//------------------- STRUCTIRES --------------------------------------------------------
enum{
	upload_type_file			= 1,
	upload_type_firmware,
};

//------------------- VARIABLES ---------------------------------------------------------
File fsUploadFile;
uint8_t uploadType				= 0;

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
	//-------------------------------------------------------------
	if( !esp::checkWebAuth( &webServer, ADMIN_LOGIN, ADMIN_PASSWORD, "Dr.Smyrke access zone", "Access denied :(" ) ) return;
	//-------------------------------------------------------------
	if( webServer.hasArg( "cmd" ) ){
		if( webServer.arg( "cmd" ) == "update" ){
			updateDevice();
			webServer.send ( 200, "text/html", "" );
			return;
		}else if( webServer.arg( "cmd" ) == "check_update" ){
			uint32_t version = atoi( FIRMWARE_SECOND_VERSION );
			strcpy( pageBuff, "{" );
				strcat( pageBuff, "\"newVersion\": " ); utoa( esp::checkingUpdate( FIRMWARE_REPOSITORY, version ), tmpVal, 10 ); strcat( pageBuff, tmpVal );
			strcat( pageBuff, "}" );

			webServer.send ( 200, "application/json", pageBuff );
			return;
		}
	}
	//-------------------------------------------------------------

	webServer.send ( 200, "text/html", "" );
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
