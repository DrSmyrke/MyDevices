#include "main.h"

//-------------------------------------------------------------------------------
DNS_Server dnsServer;
ESP8266WebServer webServer( 80 );
char pageBuff[ WEB_PAGE_BUFF_SIZE ];
IRrecv irrecv( D5 );
decode_results results;

//------------ FUNCTIONS --------------------------------------------------------

//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
void setup()
{
#ifdef __DEV
	Serial.begin( 9600 );
	Serial.println( "BOOTING..." );
#endif

	esp::init();

	IPAddress apIP( 10, 10, 10, 10 );
	bool res = esp::wifi_init( DEVICE_NAME, apIP, apIP, IPAddress( 255, 255, 255, 0 ) );
	if( res ){
		if( esp::flags.ap_mode ){
			//Captive portal redirect
			dnsServer.setErrorReplyCode( DNSReplyCode::NoError );
			dnsServer.addRecord( "*", WiFi.softAPIP() );
			dnsServer.start( DNS_PORT );
			//
			esp::pageBuff = pageBuff;
			esp::addWebServerPages( &webServer, true, true, true );
			// webServer.on( "/", handleRoot );
			webServer.begin();
		}else{
			while( !esp::isWiFiConnection() ) delay( 100 );
			ESP_DEBUG( "Send starting message\n" );
			String request = "{ \"user\": \"DrSmyrke\", \"message\": \"IR Reciever starting...\", \"key\": \"" + String( KEY ) + "\" }";
			esp::http_put( PUT_URL, request, request );
		}
	}

	irrecv.enableIRIn();
}

//-------------------------------------------------------------------------------
void loop()
{
	if( esp::flags.ap_mode ){
		webServer.handleClient();
		dnsServer.processNextRequest();
	}

	if( irrecv.decode( &results ) ){
		ESP_DEBUG( "IR Recieve\n" );
		uint16_t count = results.rawlen;
		String string = "TYPE: " + String( results.decode_type );
		if (results.decode_type == UNKNOWN) {
			string = "Unknown encoding: ";
		} else if (results.decode_type == NEC) {
			string = "Decoded NEC: ";
		} else if (results.decode_type == SONY) {
			string = "Decoded SONY: ";
		} else if (results.decode_type == RC5) {
			string = "Decoded RC5: ";
		} else if (results.decode_type == RC5X) {
			string = "Decoded RC5X: ";
		} else if (results.decode_type == RC6) {
			string = "Decoded RC6: ";
		} else if (results.decode_type == RCMM) {
			string = "Decoded RCMM: ";
		} else if (results.decode_type == PANASONIC) {
			utoa( results.address, esp::tmpVal, 16 );
			string = "Decoded PANASONIC - Address: " + String( esp::tmpVal ) + " Value: ";
		} else if (results.decode_type == LG) {
			string = "Decoded LG: ";
		} else if (results.decode_type == JVC) {
			string = "Decoded JVC: ";
		} else if (results.decode_type == AIWA_RC_T501) {
			string = "Decoded AIWA RC T501: ";
		} else if (results.decode_type == WHYNTER) {
			string = "Decoded Whynter: ";
		} else if (results.decode_type == NIKAI) {
			string = "Decoded Nikai: ";
		}

		utoa( results.value, esp::tmpVal, 16 );
		string += String( esp::tmpVal ) + " (";
		utoa( results.bits, esp::tmpVal, 10 );
		string += String( esp::tmpVal ) + " bits) Raw (";
		utoa( count, esp::tmpVal, 10 );
		string += String( esp::tmpVal ) + "): {";

		for (uint16_t i = 1; i < count; i++) {
			if (i % 100 == 0)
				yield();  // Preemptive yield every 100th entry to feed the WDT.
			if (i & 1) {
				utoa( results.rawbuf[i] * kRawTick, esp::tmpVal, 10 );
				string += String( esp::tmpVal );
			} else {
				utoa( results.rawbuf[i] * kRawTick, esp::tmpVal, 10 );
				string += ", " + String( esp::tmpVal );
			}
		}
		string += "}";
		String request = "{ \"user\": \"DrSmyrke\", \"message\": \"<b>IR Signal Recieved</b><br><code>" + String( string ) + "</code>\", \"key\": \"" + String( KEY ) + "\" }";
		int resp = esp::http_put( PUT_URL, request, request );
		ESP_DEBUG( "HTTP CODE: %d [%s]\n", resp, request.c_str() );

		irrecv.resume();
	}
}

