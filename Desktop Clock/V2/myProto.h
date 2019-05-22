#define UART_START_BYTE		0x24
#define UART_BUFF_SIZE		4
#define UART_BUFFER_MASK	UART_BUFF_SIZE - 1

#define UART_CMD_GETTIME	'g'		//0x67
#define UART_CMD_RESETTIME	'r'		//0x72
#define UART_CMD_SETTIME	't'		//0x74
#define UART_CMD_HOUR		'h'		//0x68
#define UART_CMD_MIN		'm'		//0x6D
#define UART_CMD_SEC		's'		//0x73

typedef struct {
	uint8_t rxBuff[UART_BUFF_SIZE];				// Буфер приема
	uint8_t txBuff[UART_BUFF_SIZE];
	uint8_t rxInd;								// указатель
	struct {
		uint8_t start;
		uint8_t cmd;
		uint8_t param;
		uint8_t crc;
	}pkt;
	uint8_t setTimeFlag;
	uint8_t resetTimeFlag;
	uint8_t writeTimeFlag;
} UART_Pkt;

UART_Pkt uart;

void sendUartPacket(uint8_t cmd, uint8_t param);
uint8_t chkUartPacket(void);
void parsUartPacket(uint8_t byte);
