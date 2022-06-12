#ifndef __AT_H_
#define __AT_H_

#include <stdint.h>

#define AT_M_BUFFER_SIZE			512

class ATCommand
{
	public:
		explicit ATCommand();
		void addSymbol(char sym);
		void resetBuffer(void);
		char* getData(void);
		uint8_t isOK(void);
		uint8_t isERROR(void);
		uint8_t isCommand(uint8_t skipString = false);
		uint8_t isRDY(void);
		char* getCmd(void);
		char* getValue(void);
	private:
		typedef struct {
			unsigned char rdy: 1;
			unsigned char ok: 1;
			unsigned char error: 1;
		} AT_Flags;
		char m_buffer[ AT_M_BUFFER_SIZE ];
		char m_cmd[ 10 ];
		char m_param[ 64 ];
		uint16_t m_symCounter;
		AT_Flags m_flags;
};


#endif //__AT_H_