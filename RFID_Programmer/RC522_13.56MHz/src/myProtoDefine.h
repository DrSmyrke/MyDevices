#define FALSE	0x00
#define TRUE	0x01

enum{
  START_BYTE = 0xA1,
  STOP_BYTE = 0x1A,

  CMD_ERROR = 0xFF,
  CMD_SUCCESS = 0x00,
  CMD_SET_ADDRESS = 0x09,
  CMD_READ_VERSION,
  CMD_READ_UID_AND_TYPE,
  CMD_READ_UID,
  CMD_READ_BLOCK,
  CMD_READ_PICC_TYPE,
  CMD_WRITE_UID,
  CMD_WRITE_BLOCK,
  CMD_DUMP_DATA,
};

struct Pkt{
	uint8_t cmd;
	uint8_t len;
	uint8_t data[32];
	uint8_t crc;
	uint8_t forProcessing = FALSE;
};
