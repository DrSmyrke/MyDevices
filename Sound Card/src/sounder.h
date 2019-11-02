#define NOP			255
#define NOP_FREQ	0

#define C0			0
#define C1			1
#define C2			2
#define C3			3
#define C4			4
#define C5			5
#define C6			6
#define C7			7
#define C8			8

#define CS0			10
#define CS1			11
#define CS2			12
#define CS3			13
#define CS4			14
#define CS5			15
#define CS6			16
#define CS7			17
#define CS8			18

#define D0			20
#define D1			21
#define D2			22
#define D3			23
#define D4			24
#define D5			25
#define D6			26
#define D7			27
#define D8			28

#define Eb0			30
#define Eb1			31
#define Eb2			32
#define Eb3			33
#define Eb4			34
#define Eb5			35
#define Eb6			36
#define Eb7			37
#define Eb8			38

#define E0			40
#define E1			41
#define E2			42
#define E3			43
#define E4			44
#define E5			45
#define E6			46
#define E7			47
#define E8			48

#define F0			50
#define F1			51
#define F2			52
#define F3			53
#define F4			54
#define F5			55
#define F6			56
#define F7			57
#define F8			58

#define FS0			60
#define FS1			61
#define FS2			62
#define FS3			63
#define FS4			64
#define FS5			65
#define FS6			66
#define FS7			67
#define FS8			68

#define G0			70
#define G1			71
#define G2			72
#define G3			73
#define G4			74
#define G5			75
#define G6			76
#define G7			77
#define G8			78

#define GS0			80
#define GS1			81
#define GS2			82
#define GS3			83
#define GS4			84
#define GS5			85
#define GS6			86
#define GS7			87
#define GS8			88

#define A0			90
#define A1			91
#define A2			92
#define A3			93
#define A4			94
#define A5			95
#define A6			96
#define A7			97
#define A8			98

#define BS0			100
#define BS1			101
#define BS2			102
#define BS3			103
#define BS4			104
#define BS5			105
#define BS6			106
#define BS7			107
#define BS8			108

#define B0			110
#define B1			111
#define B2			112
#define B3			113
#define B4			114
#define B5			115
#define B6			116
#define B7			117
#define B8			118






#define CASE_C0			{case C0:setFreq(16);break;}
#define CASE_C1			{case C1:setFreq(33);break;}
#define CASE_C2			{case C2:setFreq(65);break;}
#define CASE_C3			{case C3:setFreq(131);break;}
#define CASE_C4			{case C4:setFreq(261);break;}
#define CASE_C5			{case C5:setFreq(523);break;}
#define CASE_C6			{case C6:setFreq(1047);break;}
#define CASE_C7			{case C7:setFreq(2093);break;}
#define CASE_C8			{case C8:setFreq(4186);break;}

#define CASE_CS0		{case CS0:setFreq(17);break;}
#define CASE_CS1		{case CS1:setFreq(35);break;}
#define CASE_CS2		{case CS2:setFreq(69);break;}
#define CASE_CS3		{case CS3:setFreq(139);break;}
#define CASE_CS4		{case CS4:setFreq(277);break;}
#define CASE_CS5		{case CS5:setFreq(554);break;}
#define CASE_CS6		{case CS6:setFreq(1109);break;}
#define CASE_CS7		{case CS7:setFreq(2217);break;}
#define CASE_CS8		{case CS8:setFreq(4435);break;}

#define CASE_D0			{case D0:setFreq(18);break;}
#define CASE_D1			{case D1:setFreq(37);break;}
#define CASE_D2			{case D2:setFreq(73);break;}
#define CASE_D3			{case D3:setFreq(147);break;}
#define CASE_D4			{case D4:setFreq(294);break;}
#define CASE_D5			{case D5:setFreq(587);break;}
#define CASE_D6			{case D6:setFreq(1175);break;}
#define CASE_D7			{case D7:setFreq(2349);break;}
#define CASE_D8			{case D8:setFreq(4699);break;}

#define CASE_Eb0		{case Eb0:setFreq(20);break;}
#define CASE_Eb1		{case Eb1:setFreq(39);break;}
#define CASE_Eb2		{case Eb2:setFreq(78);break;}
#define CASE_Eb3		{case Eb3:setFreq(156);break;}
#define CASE_Eb4		{case Eb4:setFreq(311);break;}
#define CASE_Eb5		{case Eb5:setFreq(622);break;}
#define CASE_Eb6		{case Eb6:setFreq(1245);break;}
#define CASE_Eb7		{case Eb7:setFreq(2489);break;}
#define CASE_Eb8		{case Eb8:setFreq(4978);break;}

#define CASE_E0			{case E0:setFreq(21);break;}
#define CASE_E1			{case E1:setFreq(41);break;}
#define CASE_E2			{case E2:setFreq(82);break;}
#define CASE_E3			{case E3:setFreq(165);break;}
#define CASE_E4			{case E4:setFreq(330);break;}
#define CASE_E5			{case E5:setFreq(659);break;}
#define CASE_E6			{case E6:setFreq(1319);break;}
#define CASE_E7			{case E7:setFreq(2637);break;}
#define CASE_E8			{case E8:setFreq(5274);break;}

#define CASE_F0			{case F0:setFreq(22);break;}
#define CASE_F1			{case F1:setFreq(44);break;}
#define CASE_F2			{case F2:setFreq(87);break;}
#define CASE_F3			{case F3:setFreq(175);break;}
#define CASE_F4			{case F4:setFreq(349);break;}
#define CASE_F5			{case F5:setFreq(699);break;}
#define CASE_F6			{case F6:setFreq(1397);break;}
#define CASE_F7			{case F7:setFreq(2794);break;}
#define CASE_F8			{case F8:setFreq(5588);break;}

#define CASE_FS0		{case FS0:setFreq(23);break;}
#define CASE_FS1		{case FS1:setFreq(46);break;}
#define CASE_FS2		{case FS2:setFreq(93);break;}
#define CASE_FS3		{case FS3:setFreq(185);break;}
#define CASE_FS4		{case FS4:setFreq(370);break;}
#define CASE_FS5		{case FS5:setFreq(740);break;}
#define CASE_FS6		{case FS6:setFreq(1480);break;}
#define CASE_FS7		{case FS7:setFreq(2960);break;}
#define CASE_FS8		{case FS8:setFreq(5920);break;}

#define CASE_G0			{case G0:setFreq(25);break;}
#define CASE_G1			{case G1:setFreq(49);break;}
#define CASE_G2			{case G2:setFreq(98);break;}
#define CASE_G3			{case G3:setFreq(196);break;}
#define CASE_G4			{case G4:setFreq(392);break;}
#define CASE_G5			{case G5:setFreq(784);break;}
#define CASE_G6			{case G6:setFreq(1568);break;}
#define CASE_G7			{case G7:setFreq(3136);break;}
#define CASE_G8			{case G8:setFreq(6272);break;}

#define CASE_GS0		{case GS0:setFreq(26);break;}
#define CASE_GS1		{case GS1:setFreq(52);break;}
#define CASE_GS2		{case GS2:setFreq(104);break;}
#define CASE_GS3		{case GS3:setFreq(208);break;}
#define CASE_GS4		{case GS4:setFreq(415);break;}
#define CASE_GS5		{case GS5:setFreq(831);break;}
#define CASE_GS6		{case GS6:setFreq(1661);break;}
#define CASE_GS7		{case GS7:setFreq(3322);break;}
#define CASE_GS8		{case GS8:setFreq(6645);break;}

#define CASE_A0			{case A0:setFreq(28);break;}
#define CASE_A1			{case A1:setFreq(55);break;}
#define CASE_A2			{case A2:setFreq(110);break;}
#define CASE_A3			{case A3:setFreq(220);break;}
#define CASE_A4			{case A4:setFreq(440);break;}
#define CASE_A5			{case A5:setFreq(880);break;}
#define CASE_A6			{case A6:setFreq(1760);break;}
#define CASE_A7			{case A7:setFreq(3520);break;}
#define CASE_A8			{case A8:setFreq(7040);break;}

#define CASE_BS0		{case BS0:setFreq(29);break;}
#define CASE_BS1		{case BS1:setFreq(58);break;}
#define CASE_BS2		{case BS2:setFreq(117);break;}
#define CASE_BS3		{case BS3:setFreq(233);break;}
#define CASE_BS4		{case BS4:setFreq(466);break;}
#define CASE_BS5		{case BS5:setFreq(932);break;}
#define CASE_BS6		{case BS6:setFreq(1865);break;}
#define CASE_BS7		{case BS7:setFreq(3729);break;}
#define CASE_BS8		{case BS8:setFreq(7459);break;}

#define CASE_B0			{case B0:setFreq(31);break;}
#define CASE_B1			{case B1:setFreq(62);break;}
#define CASE_B2			{case B2:setFreq(124);break;}
#define CASE_B3			{case B3:setFreq(247);break;}
#define CASE_B4			{case B4:setFreq(494);break;}
#define CASE_B5			{case B5:setFreq(988);break;}
#define CASE_B6			{case B6:setFreq(1976);break;}
#define CASE_B7			{case B7:setFreq(3951);break;}
#define CASE_B8			{case B8:setFreq(7902);break;}

#include <avr/pgmspace.h>
#include "melody.h"

uint8_t tempo = 12;

void soundDelay( uint8_t t )
{
	while( t-- ){
		TCNT0 = 0;
		while( !TCNT0 );
		while( TCNT0 );
	}
}

void setFreq( uint16_t v )
{
	if( v > 0 ){
		TCCR1A = ( 1 << COM1A0 );
		TCCR1B = ( 1 << WGM12 ) | ( 1 << CS11 ); //			  V - prescaller
		OCR1A = F_CPU / 16 / ( v - 1 );			//F_CPU / 2 / 8 / ( v - 1 );
	}else{
		TCCR1A = TCCR1B = 0;
	}
}

void playNote( const uint8_t note, const uint8_t duration )
{
	switch( note ){
		CASE_C0;
		CASE_C1;
		CASE_C2;
		CASE_C3;
		CASE_C4;
		CASE_C5;
		CASE_C6;
		CASE_C7;
		CASE_C8;

		CASE_CS0;
		CASE_CS1;
		CASE_CS2;
		CASE_CS3;
		CASE_CS4;
		CASE_CS5;
		CASE_CS6;
		CASE_CS7;
		CASE_CS8;

		CASE_D0;
		CASE_D1;
		CASE_D2;
		CASE_D3;
		CASE_D4;
		CASE_D5;
		CASE_D6;
		CASE_D7;
		CASE_D8;

		CASE_Eb0;
		CASE_Eb1;
		CASE_Eb2;
		CASE_Eb3;
		CASE_Eb4;
		CASE_Eb5;
		CASE_Eb6;
		CASE_Eb7;
		CASE_Eb8;

		CASE_E0;
		CASE_E1;
		CASE_E2;
		CASE_E3;
		CASE_E4;
		CASE_E5;
		CASE_E6;
		CASE_E7;
		CASE_E8;

		CASE_F0;
		CASE_F1;
		CASE_F2;
		CASE_F3;
		CASE_F4;
		CASE_F5;
		CASE_F6;
		CASE_F7;
		CASE_F8;

		CASE_FS0;
		CASE_FS1;
		CASE_FS2;
		CASE_FS3;
		CASE_FS4;
		CASE_FS5;
		CASE_FS6;
		CASE_FS7;
		CASE_FS8;

		CASE_G0;
		CASE_G1;
		CASE_G2;
		CASE_G3;
		CASE_G4;
		CASE_G5;
		CASE_G6;
		CASE_G7;
		CASE_G8;

		CASE_GS0;
		CASE_GS1;
		CASE_GS2;
		CASE_GS3;
		CASE_GS4;
		CASE_GS5;
		CASE_GS6;
		CASE_GS7;
		CASE_GS8;

		CASE_A0;
		CASE_A1;
		CASE_A2;
		CASE_A3;
		CASE_A4;
		CASE_A5;
		CASE_A6;
		CASE_A7;
		CASE_A8;

		CASE_BS0;
		CASE_BS1;
		CASE_BS2;
		CASE_BS3;
		CASE_BS4;
		CASE_BS5;
		CASE_BS6;
		CASE_BS7;
		CASE_BS8;

		CASE_B0;
		CASE_B1;
		CASE_B2;
		CASE_B3;
		CASE_B4;
		CASE_B5;
		CASE_B6;
		CASE_B7;
		CASE_B8;
		case NOP:	setFreq( NOP_FREQ );	break;
	}
	soundDelay( duration * tempo );
	setFreq( NOP_FREQ );
	soundDelay( 1 );
}

void play( const uint8_t* melody, const uint16_t size )
{
	uint16_t i;
	uint8_t tempo = pgm_read_word(&(melody[0]));
	for( i = 1; i < size; i+=2 ) playNote( pgm_read_word(&(melody[i])), pgm_read_word(&(melody[i+1])) );
}
