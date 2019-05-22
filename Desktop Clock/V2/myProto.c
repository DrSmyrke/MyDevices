void sendUartPacket(uint8_t cmd, uint8_t param)
{
	uint8_t crc = ( cmd + param );
	uart_send( UART_START_BYTE );
	uart_send( cmd );
	uart_send( param );
	uart_send( crc );
	delay(100);
}
uint8_t chkUartPacket(void)
{
	uart.pkt.start = uart.rxBuff[0];
	uart.pkt.cmd = uart.rxBuff[1];
	uart.pkt.param = uart.rxBuff[2];
	uart.pkt.crc = uart.rxBuff[3];
	uint8_t crc = ( uart.pkt.cmd + uart.pkt.param );
	if( uart.pkt.start == UART_START_BYTE && uart.pkt.crc == crc ) return 1;
	return 0;
}

void parsUartPacket(uint8_t byte)
{
	if( byte == UART_START_BYTE ) uart.rxInd = 0;
	uart.rxBuff[uart.rxInd++ & UART_BUFFER_MASK] = byte;
	if( uart.rxInd == UART_BUFF_SIZE ){
		uart.rxInd = 0;
		if( chkUartPacket() ){
			switch(uart.pkt.cmd){
				case UART_CMD_GETTIME:
					sendUartPacket( UART_CMD_HOUR, DateTime.Hour );
					sendUartPacket( UART_CMD_MIN, DateTime.Min );
					sendUartPacket( UART_CMD_SEC, DateTime.Sec );
				break;
				case UART_CMD_RESETTIME: uart.resetTimeFlag = true; break;
				case UART_CMD_SETTIME:
					uart.setTimeFlag = uart.pkt.param;
				break;
				case UART_CMD_HOUR:
					if( uart.setTimeFlag ){
						DateTime.Hour = uart.pkt.param;
						uart.writeTimeFlag = true;
					}
				break;
				case UART_CMD_MIN:
					if( uart.setTimeFlag ){
						DateTime.Min = uart.pkt.param;
						uart.writeTimeFlag = true;
					}
				break;
				case UART_CMD_SEC:
					if( uart.setTimeFlag ){
						DateTime.Sec = uart.pkt.param;
						DS1302_WriteDateTime();
					}
				break;
			}
		}
	}
}
