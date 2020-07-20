#define F_CPU 8000000UL

#define APP_VERSION					1

#define MYPROTO_SEND_MY_USART_LIB
#define MYPROTO_PROCESS_FUNCTION	myproto_processCommand()

#define LED2_PIN_NUM				3
#define LED2_PORT					PORTD
#define LED2_PIN					PIND
#define LED2_INIT					pinModePD(LED2_PIN_NUM,OUTPUT)
#define LED2_READ					CheckBit(LED2_PIN,LED2_PIN_NUM)
#define LED2_ON						setZero(LED2_PORT,LED2_PIN_NUM)
#define LED2_OFF					setPlus(LED2_PORT,LED2_PIN_NUM)
#define LED2_TOGGLE					ibi(LED2_PORT,LED2_PIN_NUM)

unsigned char myproto_processCommand(void);

#include "gitbuild.h"
#include "main.h"
#include "uart.h"
#include "myProto.h"
#include "onewire.h"
//***************************************************************************
// Установки
uint8_t buff[ 32 ];
uint8_t len = 0;
//***************************************************************************
// Инициализация

inline void init(void)
{
	LED2_INIT;
	
	LED2_OFF;
}

unsigned char myproto_processCommand(void)
{
	uint8_t i, temp;
	
	switch( recvPkt.cmd ){
		case MYPROTO_CMD_SCAN:
			if( onewire_rom_skip() ){
				//onewire_send(0x44);
				_delay_ms(900);
				recvPkt.len = 0;
			}
			onewire_enum_init();
			while( onewire_enum_next() ){
				uint8_t crc = 0;
				for( i = 0; i < 9; i++ ){
					recvPkt.data[recvPkt.len++] = onewire_getCode(i);
				}
			}
		break;
		case MYPROTO_CMD_SEND_READ:
			if( recvPkt.len < 2 ){
				recvPkt.cmd = MYPROTO_CMD_ERROR;
				recvPkt.len = pack_string( recvPkt.data, "DATA LENGTH < 2 BYTES [len read, data...]" );
			}else{
				recvPkt.cmd = MYPROTO_CMD_SUCCESS;
				temp = recvPkt.data[0];
				for( i = 1; i < recvPkt.len; i++ ){
					onewire_send( recvPkt.data[i] );
				}
				recvPkt.len = temp;
				for( i = 0; i < recvPkt.len; i++ ){
					recvPkt.data[i] = onewire_read();
				}
			}
		break;
		case MYPROTO_CMD_SEND:
			for( i = 0; i < recvPkt.len; i++ ){
				onewire_send( recvPkt.data[i] );
			}
			recvPkt.cmd = MYPROTO_CMD_SUCCESS;
			recvPkt.len = 0;
		break;
		case MYPROTO_CMD_READ:
			recvPkt.cmd = MYPROTO_CMD_SUCCESS;
			if( recvPkt.len == 1 ){
				recvPkt.len = recvPkt.data[0];
				for( i = 0; i < recvPkt.len; i++ ){
					recvPkt.data[i] = onewire_read();
				}
			}else{
				recvPkt.len = 1;
				recvPkt.data[0] = onewire_read();
			}
		break;
		case MYPROTO_CMD_SET_ADDRESS:
			if( recvPkt.len != 8 ){
				recvPkt.cmd = MYPROTO_CMD_ERROR;
				recvPkt.len = pack_string( recvPkt.data, "DATA LENGTH != 8 BYTES" );
			}else{
				if( onewire_isBroadcast( recvPkt.data ) ){
					onewire_rom_skip();
					uart_send(0xFE);
				}else{
					onewire_rom_match( recvPkt.data );
					uart_send(0xF1);
				}
				recvPkt.cmd = MYPROTO_CMD_SUCCESS;
				recvPkt.len = 0;
			}
		break;
	}
	
	return 1;
}

int main(void)
{
	init();
	uart_init();
	myproto_init( ONEWIRE_PROGRAMMER_TYPE, 1 );
	onewire_master_init();
	
	sei();

	/*
	delay(65000);
	
	recvPkt.data[0] = 0x03;
	recvPkt.data[1] = 0x04;
	len = myproto_packData( buff, 0x01, recvPkt.data, 2 );
	uart_send_buff( buff, len );
	*/
	
	while(1){
		if( uart_isReadAvailable() ){
			len = uart_readData( buff );
			if( len ){
				myproto_setRXData( buff, &len );
			}
			//uart_send_buff( buff, len );
			//uart_send( 0xff );
			//uart_send( 0x00 );
			//uart_send( 0xff );
		}

		myproto_process();
		
		delay( 3000 );
	}
}
