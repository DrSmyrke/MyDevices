#include "AT.h"
#include <string.h>



//------------------------------------------------------
ATCommand::ATCommand()
{
	m_flags.rdy				= 0;
	resetBuffer();
}





//------------------------------------------------------
void ATCommand::addSymbol(char sym)
{
	if( m_flags.ok || m_flags.error || m_symCounter >= AT_M_BUFFER_SIZE ){
		resetBuffer();
	}

	m_buffer[ m_symCounter++ ]					= sym;

	if( m_symCounter >= 6 ){
		char* buff = m_buffer + m_symCounter - 6;
		if( buff[ 0 ] == '\r' && buff[ 1 ] == '\n' && buff[ 2 ] == 'O' && buff[ 3 ] == 'K' && buff[ 4 ] == '\r' && buff[ 5  ] == '\n' ){
			m_flags.ok							= 1;
			m_symCounter						-= 6;
			buff[ 0 ]							= '\0';
			while( m_symCounter-- ){
				if( m_buffer[ m_symCounter ] == '\r' || m_buffer[ m_symCounter ] == '\n' ){
					m_buffer[ m_symCounter ] = '\0';
				}else{
					break;
				}
			}
		}

		if( m_symCounter >= 7 ){
			buff = m_buffer + m_symCounter - 7;
			if( buff[ 0 ] == '\r' && buff[ 1 ] == '\n' && buff[ 2 ] == 'R' && buff[ 3 ] == 'D' && buff[ 4 ] == 'Y' && buff[ 5 ] == '\r' && buff[ 6 ] == '\n' ){
				m_flags.rdy						= 1;
				resetBuffer();
			}

			if( m_symCounter >= 9 ){
				buff = m_buffer + m_symCounter - 9;
				if( buff[ 0 ] == '\r' && buff[ 1 ] == '\n' && buff[ 2 ] == 'E' && buff[ 3 ] == 'R' && buff[ 4 ] == 'R' && buff[ 5 ] == 'O' && buff[ 6 ] == 'R' && buff[ 7 ] == '\r' && buff[ 8  ] == '\n' ){
					m_flags.error				= 1;
					m_symCounter				-= 9;
					buff[ 0 ]					= '\0';
					while( m_symCounter-- ){
						if( m_buffer[ m_symCounter ] == '\r' || m_buffer[ m_symCounter ] == '\n' ){
							m_buffer[ m_symCounter ] = '\0';
						}else{
							break;
						}
					}
				}
			}
		}
	}	
}

//------------------------------------------------------
void ATCommand::resetBuffer(void)
{
	m_symCounter					= 0;
	m_flags.ok						= 0;
	m_flags.error					= 0;

	for( uint16_t i = 0; i < AT_M_BUFFER_SIZE; i++ ) m_buffer[ i ] = '\0';
}

//------------------------------------------------------
char* ATCommand::getData(void)
{
	return m_buffer;
}

//------------------------------------------------------
uint8_t ATCommand::isOK(void)
{
	return m_flags.ok;
}

//------------------------------------------------------
uint8_t ATCommand::isERROR(void)
{
	return m_flags.error;
}

//------------------------------------------------------
uint8_t ATCommand::isCommand(uint8_t skipString)
{
	uint8_t res						= 0;
	uint8_t cmdInd					= 0;
	uint8_t valInd					= 0;
	uint8_t cmdFlag					= 0;
	uint8_t endFlag					= 0;
	uint8_t skipFlag				= skipString;
	uint16_t i						= 0;

	for( i = 0; i < AT_M_BUFFER_SIZE; i++ ){
		char sym = m_buffer[ i ];

		if( sym == '\0' ){
			if( skipFlag ) endFlag = 1;
			break;
		}
		if( sym == '\r' ) continue;

		if( skipString ){
			if( sym == '\n' ) skipString = 0;
			continue;
		}
		
		if( sym == ':' && !cmdFlag ){
			if( ( i + 1 ) < AT_M_BUFFER_SIZE ){
				if( m_buffer[ i + 1 ] == ' ' ){
					m_cmd[ cmdInd++ ] = '\0';
					cmdFlag = 1;
					continue;
				}
			}
		}
		

		if( !cmdFlag ){
			if( sym == '\n' && cmdInd == 0 ) continue;
			m_cmd[ cmdInd++ ] = sym;
		}else{
			if( sym == ' ' && valInd == 0 ) continue;
			if( sym == '\n' ){
				endFlag = 1;
				break;
			}
			m_param[ valInd++ ] = sym;
		}
	}

	// if( i >= 2 ){
	// 	char* buff = m_buffer + i - 2 ;
	// 	if( buff[ 0 ] == '\r' && buff[ 1 ] == '\n' ){
	// 		rnCounter = 1;
	// 	}
	// }

	if( cmdInd && valInd && m_cmd[ 0 ] == '+' && endFlag ) res = 1;

	m_param[ valInd++ ] = '\0';

	return res;
}

//------------------------------------------------------
uint8_t ATCommand::isRDY(void)
{
	return m_flags.rdy;
}

//------------------------------------------------------
char* ATCommand::getCmd(void)
{
	return m_cmd;
}

//------------------------------------------------------
char* ATCommand::getValue(void)
{
	return m_param;
}

//------------------------------------------------------
//------------------------------------------------------
//------------------------------------------------------
//------------------------------------------------------
//------------------------------------------------------
//------------------------------------------------------
//------------------------------------------------------
//------------------------------------------------------
//------------------------------------------------------
//------------------------------------------------------
//------------------------------------------------------
//------------------------------------------------------
