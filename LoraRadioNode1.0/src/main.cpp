#include "main.h"
#include <LoRaNow.h>
#include <avr/wdt.h>

//------------------- DEFINES -----------------------------


//------------------- VARIABLES ---------------------------




//----------------------------------------------------------
void onMessage(uint8_t *buffer, size_t size)
{
	unsigned long id = LoRaNow.id();
	byte count = LoRaNow.count();
  
	Serial.print("Node Id: ");
	Serial.print(id, HEX);
	Serial.print(" Count: ");
	Serial.print(count);
	Serial.print(" RSSI: ");
	Serial.print(LoRa.packetRssi());
	Serial.print(" Message: ");
	for( uint8_t i = 0; i < size; i++ ){
		if( i > 0 ) Serial.write( ' ' );
		if( buffer[ i ] < 16 )   Serial.write( '0' );
		Serial.print( buffer[ i ], HEX );
    }
	// Serial.write(buffer, size);
	Serial.println();
}

//----------------------------------------------------------
void onSleep()
{
	Serial.println("Sleep");
	delay(5000); // "kind of a sleep"
	Serial.println("Send Message");
	LoRaNow.print("LoRaNow Node Message ");
	LoRaNow.print(millis());
	LoRaNow.send();
}

//----------------------------------------------------------
void setup()
{
	Serial.begin( 115200 );

	LoRaNow.setFrequencyEU();
	if( !LoRaNow.begin() ){
		Serial.println("LoRa init failed.");
	}else{
		Serial.println("LoRa init OK");
	}
	LoRaNow.onMessage(onMessage);

#ifdef SNIFFER_MODE
	Serial.println( F("Starting LoRa sniffer...") );
	LoRaNow.gateway();
#endif
#ifdef SPAMMER_MODE
	Serial.println( F("Starting LoRa spammer...") );
	LoRaNow.onSleep(onSleep);
#endif
	LoRaNow.showStatus(Serial);

	wdt_enable( WDTO_8S );
}

//----------------------------------------------------------
void loop()
{
	wdt_reset();
	LoRaNow.loop();
}
