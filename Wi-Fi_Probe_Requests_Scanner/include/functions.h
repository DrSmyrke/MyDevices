#ifndef _MAIN_FUNCTIONS_H_
#define _MAIN_FUNCTIONS_H_

#include "main.h"

//-------------------------------------------------------------------------------
#define WIFI_MAC_LEN								6
#define WIFI_SSID_LEN								33
#define WIFI_MANAGEMENT_MAC_HEADER_SIZE				36

// - DS Field
#define WIFI_DS_NO									0
#define WIFI_DS_TO									1
#define WIFI_DS_FROM								2
#define WIFI_DS_TOFROM								3

// - Frame types
#define WIFI_MANAGEMENT								0
#define WIFI_CONTROL								1
#define WIFI_DATA									2

// - Frame subtypes
#define WIFI_MANAGEMENT_ASSOCIATION_REQUEST			0
#define WIFI_MANAGEMENT_ASSOCIATION_RESPONSE		1
#define WIFI_MANAGEMENT_REASSOCIATION_REQUEST		2
#define WIFI_MANAGEMENT_REASSOCIATION_RESPONSE		3
#define WIFI_MANAGEMENT_PROBE_REQUEST				4
#define WIFI_MANAGEMENT_PROBE_RESPONSE				5
#define WIFI_MANAGEMENT_TIMMING_ADVERTISEMENT		6
#define WIFI_MANAGEMENT_RESERVED1					7
#define WIFI_MANAGEMENT_BEACON						8
#define WIFI_MANAGEMENT_ATIM						9
#define WIFI_MANAGEMENT_DISASSOCIATION				10
#define WIFI_MANAGEMENT_AUTHENTICATION				11
#define WIFI_MANAGEMENT_DEAUTHENTICATION			12
#define WIFI_MANAGEMENT_ACTION						13
#define WIFI_MANAGEMENT_ACTION_NO_ACK				14
#define WIFI_MANAGEMENT_RESERVED2					15

#define WIFI_CONTROL_RESERVED1						0
#define WIFI_CONTROL_RESERVED2						1
#define WIFI_CONTROL_RESERVED3						2
#define WIFI_CONTROL_RESERVED4						3
#define WIFI_CONTROL_RESERVED5						4
#define WIFI_CONTROL_RESERVED6						5
#define WIFI_CONTROL_RESERVED7						6
#define WIFI_CONTROL_CONTROL_WRAPPER				7
#define WIFI_CONTROL_BLOCK_ACK_REQUEST				8
#define WIFI_CONTROL_BLOCK_ACK						9
#define WIFI_CONTROL_PS_POLL						10
#define WIFI_CONTROL_RTS							11
#define WIFI_CONTROL_CTS							12
#define WIFI_CONTROL_ACK							13
#define WIFI_CONTROL_CF_END							14
#define WIFI_CONTROL_CF_END_CF_ACK					15

#define WIFI_DATA_DATA								0
#define WIFI_DATA_DATA_CF_ACK						1
#define WIFI_DATA_DATA_CF_POLL						2
#define WIFI_DATA_DATA_CF_ACK_CF_POLL				3
#define WIFI_DATA_NULL								4
#define WIFI_DATA_CF_ACK							5
#define WIFI_DATA_CF_POLL							6
#define WIFI_DATA_CF_ACK_CF_POLL					7
#define WIFI_DATA_QOS_DATA							8
#define WIFI_DATA_QOS_DATA_CF_ACK					9
#define WIFI_DATA_QOS_DATA_CF_ACK_CF_POLL			10
#define WIFI_DATA_QOS_NULL							11
#define WIFI_DATA_RESERVED1							12
#define WIFI_DATA_QOS_CF_POLL						13
#define WIFI_DATA_QOS_CF_ACK_CF_POLL				14

//-------------------------------------------------------------------------------
struct RxControl
{
	signed rssi: 8;
	unsigned rate: 4;
	unsigned is_group: 1;
	unsigned: 1;
	unsigned sig_mode: 2;
	unsigned legacy_length: 12;
	unsigned damatch0: 1;
	unsigned damatch1: 1;
	unsigned bssidmatch0: 1;
	unsigned bssidmatch1: 1;
	unsigned MCS: 7;
	unsigned CWB: 1;
	unsigned HT_length: 16;
	unsigned Smoothing: 1;
	unsigned Not_Sounding: 1;
	unsigned: 1;
	unsigned Aggregation: 1;
	unsigned STBC: 2;
	unsigned FEC_CODING: 1;
	unsigned SGI: 1;
	unsigned rxend_state: 8;
	unsigned ampdu_cnt: 8;
	unsigned channel: 4;
	unsigned: 12;
};

struct LenSeq
{
	uint16_t length;
	uint16_t seq;
	uint8_t  address3[6];
};

struct sniffer_buf
{
	struct RxControl rx_ctrl;
	uint8_t buf[36];
	uint16_t cnt;
	struct LenSeq lenseq[1];
};

struct sniffer_buf2
{
	struct RxControl rx_ctrl;
	uint8_t buf[112];
	uint16_t cnt;
	uint16_t len;
};

struct wifi_frame_info
{
	uint8_t frametype;
	uint8_t framesubtype;
	uint8_t receiveraddr[WIFI_MAC_LEN];
	uint8_t destinationaddr[WIFI_MAC_LEN];
	uint8_t transmitteraddr[WIFI_MAC_LEN];
	uint8_t sourceaddr[WIFI_MAC_LEN];
	uint8_t bssid[WIFI_MAC_LEN];  
	uint8_t ssid[WIFI_SSID_LEN];
	uint8_t ssid_length;
	unsigned channel;
	signed rssi;
	uint16_t seq_num;
	uint8_t raw[512];
	uint8_t raw_length;
	bool isvalid;
};

typedef void (*WIFI_CB_T) (wifi_frame_info *frame);

//-------------------------------------------------------------------------------
bool WiFiConnectionState(void);
bool is_clientMode(void);
void wifi_STA_init(void);
void wifi_AP_init(void);
void wifi_sniffer_init(void);
void wifi_rx_cb(uint8_t *buf, uint16_t len);
void wifi_parsing_data(uint8_t *frame, signed rssi, uint16_t len);
bool maccmp(uint8_t *mac1, uint8_t *mac2);




#endif //_MAIN_FUNCTIONS_H_
