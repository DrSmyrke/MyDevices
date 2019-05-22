#include "myProtoDefine.h"

Pkt recvPkt;
Pkt sendPkt;

uint8_t MyProto_parsData( Pkt &pkt, uint8_t* buff, const uint8_t len )
{
	uint8_t crc = 0;

	if( len < 5 ) return FALSE;
	if( buff[0] != START_BYTE ) return FALSE;

	pkt.cmd = buff[1];
	pkt.len = buff[2];

	if( len < 5 + pkt.len ) return FALSE;
	uint8_t	i = 3;
	for( i = 3; i < pkt.len + 3; i++ ){
		pkt.data[i-3] = buff[i];
		crc += pkt.data[i-3];
	}

	pkt.crc = buff[i];
	if( buff[++i] != STOP_BYTE ) return FALSE;
	crc += pkt.cmd;
	crc += pkt.len;

	if( crc != pkt.crc ) return FALSE;
	pkt.forProcessing = TRUE;
	return TRUE;
}

void MyProto_send( const uint8_t cmd, uint8_t* buff, const uint8_t len )
{
  sendPkt.cmd = cmd;
  sendPkt.len = len;
  sendPkt.crc = sendPkt.cmd + sendPkt.len;
  for(uint8_t i = 0; i < len; i++){
    sendPkt.data[i] = buff[i];
    sendPkt.crc += sendPkt.data[i];
  }
  
  Serial.write(START_BYTE);
  Serial.write(sendPkt.cmd);
  Serial.write(sendPkt.len);
  for(uint8_t i = 0; i < sendPkt.len; i++) Serial.write(sendPkt.data[i]);
  Serial.write(sendPkt.crc);
  Serial.write(STOP_BYTE);
}
