#ifndef __AT_H_
#define __AT_H_

#include <stdint.h>
#include <SoftwareSerial.h>

#define AT_M_BUFFER_SIZE			512

typedef std::function<bool(void)> THandlerFunction;

class AT
{
	public:
		explicit AT(SoftwareSerial* serial = nullptr, THandlerFunction cb_processing = nullptr);
		void addSymbol(char sym);
		void resetBuffer(void);
		char* getData(void);
		uint8_t isOK(void);
		uint8_t isERROR(void);
		uint8_t isCommand(uint8_t skipString = false);
		uint8_t isRDY(void);
		uint8_t isRing(void);
		char* getCmd(void);
		char* getValue(void);
		bool sendATCommand(const char* cmd, bool waiting, bool processing = false);
		void dataRead(bool processing = true);
	private:
		typedef struct {
			unsigned char rdy: 1;
			unsigned char ok: 1;
			unsigned char error: 1;
			unsigned char ring: 1;
		} AT_Flags;
		char m_buffer[ AT_M_BUFFER_SIZE ];
		char m_cmd[ 10 ];
		char m_param[ 64 ];
		uint16_t m_symCounter;
		AT_Flags m_flags;
		SoftwareSerial* m_pSerialPort;
		long m_timer;
		THandlerFunction m_dataProcess;
};


#endif //__AT_H_