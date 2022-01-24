	#define	DS1302
//#define	DS1307



#include <Arduino.h>
#include <GyverTimers.h>
#include <Time.h>

#if defined( DS1307 )
	#include <Wire.h>
	#include <DS1307.h>
	#define TIMEZONE 1
	DS1307 *time;
#elif defined( DS1302 )
	#include <ThreeWire.h>  
	#include <RtcDS1302.h>
	ThreeWire myWire( 4, 5, 2 ); // IO/DAT, SCLK/CLK, CE/RST
	RtcDS1302<ThreeWire> Rtc( myWire );
#endif

#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel strip = Adafruit_NeoPixel(76, 6, NEO_GRB + NEO_KHZ800);	//count, pin




#if defined( DS1307 )
	
#elif defined( DS1302 )

#endif


#if defined( DS1307 )
void updateTime()
{
	btime_t bt;

	bt.year = 2017;
	bt.mon = MAY;
	bt.mday = 27;
	bt.hour = 14;
	bt.min = 25;
	bt.sec = 10;
	bt.wday = SATURDAY;
	bt.zone = TIMEZONE;
	time->mktime( bt );
	time->saveToRTC();  
}

void serialPrintTime(btime_t bt)
{
	Serial.print(bt.year);
	Serial.print("-");
	Serial.print(bt.mon);
	Serial.print("-");
	Serial.print(bt.mday);
	Serial.print(" ");
	Serial.print(bt.hour);
	Serial.print(":");
	Serial.print(bt.min);
	Serial.print(":");
	Serial.print(bt.sec);
	Serial.print(" TZ(");
	Serial.print(bt.zone);
	Serial.println(")");
}
#elif defined( DS1302 )
#define countof(a) (sizeof(a) / sizeof(a[0]))
void printDateTime( const RtcDateTime& dt )
{
	char datestring[20];

	snprintf_P(datestring, 
			countof(datestring),
			PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
			dt.Month(),
			dt.Day(),
			dt.Year(),
			dt.Hour(),
			dt.Minute(),
			dt.Second() );
	Serial.println(datestring);
}
RtcDateTime compiled = RtcDateTime( __DATE__, __TIME__ );
#endif

//----------------------------------------------------
static uint8_t second = 0;
static uint8_t secondFlag = 0;
static uint8_t rainbow = 0;
static uint8_t txTDbuff[ 8 ];
static bool pcMode = false;
static uint8_t sec = 0;
static uint8_t min = 0;
static uint8_t hour = 0;
static uint8_t day = 0;
static uint8_t mon = 0;
static uint16_t year = 0;
//----------------------------------------------------




void setup()
{
	Serial.begin( 115200 );

	Timer1.setFrequency( 1 );
	Timer1.enableISR( CHANNEL_A );

#if defined( DS1307 )
	Wire.begin();
	time = new DS1307( TIMEZONE );
#elif defined( DS1302 )
	Rtc.Begin();
	Serial.print("compiled: ");
	Serial.print(__DATE__);
	Serial.println(__TIME__);
	
	printDateTime( compiled );
	Serial.println();

	if( !Rtc.IsDateTimeValid() ){
		// Common Causes:
		//    1) first time you ran and the device wasn't running yet
		//    2) the battery on the device is low or even missing

		Serial.println( "RTC lost confidence in the DateTime!" );
		Rtc.SetDateTime( compiled );
	}

	if( Rtc.GetIsWriteProtected() ){
		Serial.println( "RTC was write protected, enabling writing now" );
		Rtc.SetIsWriteProtected( false );
	}

	if( !Rtc.GetIsRunning() ){
		Serial.println( "RTC was not actively running, starting now" );
		Rtc.SetIsRunning( true );
	}

	RtcDateTime now = Rtc.GetDateTime();
	if( now < compiled ){
		Serial.println( "RTC is older than compile time!  (Updating DateTime)" );
		Rtc.SetDateTime( compiled );
	}
#endif
//	updateTime();
	strip.begin();
	strip.show();
	strip.setBrightness( 10 );
}

void clearStrip(){
	for( uint16_t i = 0; i < strip.numPixels(); i++ ){
		strip.setPixelColor( i, strip.Color( 0, 0, 0 ) );
	}
}


ISR(TIMER1_A)
{
	secondFlag = 1;
	second++;
}

uint32_t getPixelColor(uint16_t i)
{
	byte WheelPos;
	WheelPos = ( rainbow + i ) & 255;
	if( WheelPos < 85 ){
		return strip.Color( WheelPos * 3, 255 - WheelPos * 3, 0 );
	}else if( WheelPos < 170 ){
		WheelPos -= 85;
		return strip.Color( 255 - WheelPos * 3, 0, WheelPos * 3 );
	}else{
		WheelPos -= 170;
		return strip.Color( 0, WheelPos * 3, 255 - WheelPos * 3 );
	}
}

void litPixels(uint16_t start, uint16_t finish)
{
	for( uint16_t i = start; i <= finish; i++ ){
		strip.setPixelColor( i, getPixelColor( i ) );
	}
}

void updateHour(uint8_t hour)
{
	switch( hour ){
		case 1:
			litPixels( 40, 42 ); // ONE
		break;
		case 2:
			litPixels( 34, 36 ); // TWO
		break;
		case 3:
			litPixels( 43, 46 ); // THREE
		break;
		case 4:
			litPixels( 31, 33 ); // FOUR
		break;
		case 5:
			litPixels( 29, 30 ); // FIVE
		break;
		case 6:
			litPixels( 19, 20 ); // SIX
		break;
		case 7:
			litPixels( 21, 24 ); // SEVEN
		break;
		case 8:
			litPixels( 25, 28 ); // EIGHT
		break;
		case 9:
			litPixels( 16, 18 ); // NINE
		break;
		case 10:
			litPixels( 14, 15 ); // TEN
		break;
		case 11:
			litPixels( 10, 13 ); // ELEVEN
		break;
		case 12:
		case 0:
			litPixels( 0, 4 ); // TWELVE 
		break;
	}
}

void updatePixels(uint8_t hour, uint8_t min)
{
	clearStrip();
	litPixels( 74, 76 ); // IT'S

	if( 0 == hour ){
		hour = 12;
	}else{
		hour = hour%12;
	}

	if( min < 35 ){
		updateHour( hour );
    	if(min >= 0 && min <5){
			litPixels( 5, 9 ); // O'CLOCK
		}else{
			if( min < 30 ){
				if( min >= 5 && min < 10 ){
					litPixels( 55, 57 ); // FIVE
					litPixels( 49, 54 ); // MINUTES
				}else if( min >= 10 && min < 15 ){
					litPixels( 69, 70 ); // TEN
					litPixels( 49, 54 ); // MINUTES
				}else if( min >= 15 && min < 20 ){
					litPixels( 58, 63 ); // QUARTER
				}else if( min >= 20 && min < 25 ){
					litPixels( 64, 68 ); // TWENTY
					litPixels( 49, 54 ); // MINUTES
				}else if( min >= 25 && min < 30 ){
					litPixels( 64, 68 ); // TWENTY
					litPixels( 55, 57 ); // FIVE
					litPixels( 49, 54 ); // MINUTES
				}
			}else if( min >= 30 && min < 35 ){
				litPixels( 71, 73 ); // HALF
			}
			litPixels( 37, 39 ); // PAST
		}
	}else{
		if( hour < 12 ){
			updateHour( hour + 1 );
		}else{
			updateHour( 1 );
		}
		if( min >= 35 && min < 40 ){
			litPixels( 64, 68 ); // TWENTY
			litPixels( 55, 57 ); // FIVE
			litPixels( 49, 54 ); // MINUTES
		}else if( min >= 40 && min < 45 ){
			litPixels( 64, 68 ); // TWENTY
			litPixels( 49, 54 ); // MINUTES
		}else if( min >= 45 && min < 50 ){
			litPixels( 58, 63 ); // QUARTER
		}else if( min >= 50 && min < 55 ){
			litPixels( 69, 70 ); // TEN
			litPixels( 49, 54 ); // MINUTES
		}else if( min >= 55 && min < 60 ){
			litPixels( 55, 57 ); // FIVE
			litPixels( 49, 54 ); // MINUTES
		}
		litPixels( 47, 48 ); // TO
	}

	strip.show();
	rainbow += 1;
}

void demo()
{
	while( true ) {
		for( uint8_t hour = 1; hour <= 12; hour++ ){
			for( uint8_t min = 0; min < 60; min++ ){
				updatePixels( hour, min );
				rainbow += 1;
				delay( 500 );
			}
		}
	}
}


//----------------------------------------------------
void updateDateTime(void)
{
#if defined( DS1307 )
	btime_t bt;
	time->loadFromRTC();
	bt = time->getBTime();
#elif defined( DS1302 )
	RtcDateTime now = Rtc.GetDateTime();
	if( !now.IsValid() ){
		sec = 0;
		min = 0;
		hour = 0;
		day = 0;
		mon = 0;
		year = 0;
	}else{
		sec = now.Second();
		min = now.Minute();
		hour = now.Hour();
		day = now.Day();
		mon = now.Month();
		year = now.Year();
	}
#endif

	txTDbuff[ 0 ] = 0xA1;
	txTDbuff[ 1 ] = year >> 8;
	txTDbuff[ 2 ] = year;
	txTDbuff[ 3 ] = mon;
	txTDbuff[ 4 ] = day;
	txTDbuff[ 5 ] = hour;
	txTDbuff[ 6 ] = min;
	txTDbuff[ 7 ] = sec;
}

//----------------------------------------------------
void loop()
{
	//demo();

	// ---------- CheckIn computer connection for programming ----------------------
	if( Serial.available() ){
		uint8_t sym = Serial.read();

		if( sym == 0xF1 ){
			Serial.write( 0xCF );
			pcMode = true;
			strip.setBrightness( 5 );
		}else if( sym == 0xAA ){
			delay( 100 );
			uint8_t i = 0;
			while( Serial.available() && i < 7 ){
				int byte = Serial.read();
				if( byte == -1 ) break;
				txTDbuff[ i++ ] = (uint8_t)byte;
			}

			if( i == 7 ){
				year = ( ( txTDbuff[ 0 ] << 8 ) | ( txTDbuff[ 1 ] ) );
#if defined( DS1307 )
#elif defined( DS1302 )
			RtcDateTime dt( year, txTDbuff[ 2 ], txTDbuff[ 3 ], txTDbuff[ 4 ], txTDbuff[ 5 ], txTDbuff[ 6 ] );
			Rtc.SetDateTime( dt );
#endif
			}
		}
	}




	if( secondFlag ){
		secondFlag = 0;
		
		updateDateTime();

		if( pcMode ){
			Serial.write( txTDbuff, sizeof( txTDbuff ) );
		}
		





		if( second++ >= 15 ){
			second = 0;

			if( pcMode ){
				pcMode = false;
				Serial.write( 0xAF );
			}
			
			updatePixels( hour, min );
		}
	}
}
