#include "myDevicesTypes.h"
#include "gitbuild.h"

#define MYPROTO_BUFF_SIZE			32
#define MYPROTO_BUFF_MASK			MYPROTO_BUFF_SIZE - 1
#define MYPROTO_VERSION				1

#define MYPROTO_SEND_MY_USART_LIB
#ifndef APP_VERSION
	#define APP_VERSION				0
#endif

volatile uint8_t myproto_rx_buff[MYPROTO_BUFF_SIZE];
volatile uint8_t myproto_rx_rIndx	= 0;
volatile uint8_t myproto_rx_wIndx	= 0;
volatile uint8_t myproto_type		= 0;
volatile uint8_t myproto_id			= 0;

enum{
  MYPROTO_START_BYTE				= 0xA1,
  MYPROTO_STOP_BYTE					= 0x1A,

  MYPROTO_CMD_ERROR					= 0xFF,
  MYPROTO_CMD_ERROR_CRC				= 0xFE,
  
  MYPROTO_CMD_SUCCESS				= 0x00,
  MYPROTO_CMD_GET_ID,
  MYPROTO_CMD_SCAN,
  MYPROTO_CMD_READ,
  MYPROTO_CMD_SEND,
  MYPROTO_CMD_SEND_READ,			// 0x05
  MYPROTO_CMD_SET_ADDRESS,			
};

struct MyProtoPkt{
	uint8_t cmd;
	uint8_t len;
	uint8_t data[MYPROTO_BUFF_SIZE];
	uint8_t crc;
	uint8_t processF;
	uint8_t readlen;
	struct Flags {
		unsigned valid			: 1;
		unsigned crcError		: 1;
	} flags;
} recvPkt;




void myproto_processCommand(void);

inline void myproto_send(const uint8_t byte)
{
	#ifdef MYPROTO_SEND_MY_USART_LIB
		uart_send( byte );
	#endif
}

void myproto_init( const uint8_t type, const uint8_t id )
{
	recvPkt.processF		= 0;
	recvPkt.readlen			= 0;
	myproto_type			= type;
	myproto_id				= id;
}

void myproto_setRXData( uint8_t* buff, const uint8_t *len )
{
	uint8_t i;
	for( i = 0; i < *len; i++ ){
		myproto_rx_buff[ myproto_rx_wIndx++ & MYPROTO_BUFF_MASK ] = buff[i];
	}
}

void myproto_sendData( const uint8_t cmd, uint8_t* data, const uint8_t len )
{
	uint8_t crc		= 0;
	uint8_t i;
	
	crc				+= cmd;
	crc				+= len;
	
	myproto_send( MYPROTO_START_BYTE );
	myproto_send( cmd );
	myproto_send( len );
	for( i = 0; i < len; i++ ){
		crc += data[i];
		myproto_send( data[i] );
	}
	myproto_send( crc );
	myproto_send( MYPROTO_STOP_BYTE );
}

uint8_t myproto_packData( uint8_t* out_buff, const uint8_t cmd, uint8_t* data, const uint8_t len )
{
	uint8_t crc		= 0;
	uint8_t ch		= 0;
	uint8_t i;
	
	crc				+= cmd;
	crc				+= len;
	
	out_buff[ch++]	= MYPROTO_START_BYTE;
	out_buff[ch++]	= cmd;
	out_buff[ch++]	= len;
	for( i = 0; i < len; i++ ){
		out_buff[ch++] = data[i];
		crc += data[i];
	}
	out_buff[ch++]	= crc;
	out_buff[ch++]	= MYPROTO_STOP_BYTE;
	
	return ch;
}

void myproto_process()
{
	while( myproto_rx_wIndx != myproto_rx_rIndx ){
		uint8_t byte = myproto_rx_buff[ myproto_rx_rIndx++ & UART_BUFF_MASK ];
		
		if( !recvPkt.processF ){
			if( byte == MYPROTO_START_BYTE ){
				recvPkt.readlen = 0;
				recvPkt.processF++;
			}
			continue;
		}
		
		switch( recvPkt.processF ){
			case 1:
				recvPkt.cmd = byte;
				recvPkt.processF++;
			break;
			case 2:
				recvPkt.len = byte;
				recvPkt.readlen = 0;
				recvPkt.processF++;
				if( recvPkt.len == 0 ) recvPkt.processF++;
			break;
			case 3:
				recvPkt.data[recvPkt.readlen++] = byte;
				if( recvPkt.readlen >= recvPkt.len ){
					recvPkt.data[recvPkt.readlen] = 0x00;
					recvPkt.processF++;
				}
			break;
			case 4:
				recvPkt.crc = byte;
				recvPkt.processF++;
				continue;
			break;
		}
		
		if( recvPkt.processF == 5 ){
			recvPkt.processF = 0;
			
			if( byte == MYPROTO_STOP_BYTE ){

				uint8_t i;
				uint8_t crc = 0;
				crc += recvPkt.cmd;
				crc += recvPkt.len;
				for( i = 0; i < recvPkt.len; i++ ){
					crc += recvPkt.data[i];
				}
				recvPkt.flags.crcError = ( crc == recvPkt.crc ) ? 0 : 1;
				recvPkt.flags.valid = 1;
				break;
			}
			
			recvPkt.flags.valid = 0;
		}
	}
	
	if( recvPkt.flags.valid ){
		recvPkt.flags.valid = 0;

		if( recvPkt.flags.crcError ){
			recvPkt.cmd		= MYPROTO_CMD_ERROR_CRC;
			recvPkt.len		= 1;
			recvPkt.data[0] = recvPkt.crc;
			myproto_sendData( recvPkt.cmd, recvPkt.data, recvPkt.len );
			return;
		}

		if( recvPkt.cmd == MYPROTO_CMD_GET_ID ){
			recvPkt.data[0]	= myproto_type;
			recvPkt.data[1]	= myproto_id;
			recvPkt.data[2]	= APP_VERSION;
			recvPkt.data[3]	= MYPROTO_VERSION;
			recvPkt.data[4]	= BUILD_NUM<<8;
			recvPkt.data[5]	= BUILD_NUM;
			recvPkt.len		= 6;
			myproto_sendData( recvPkt.cmd, recvPkt.data, recvPkt.len );
			return;
		}
		
		myproto_processCommand();
		
		myproto_sendData( recvPkt.cmd, recvPkt.data, recvPkt.len );
	}
}
