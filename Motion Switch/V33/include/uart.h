#ifndef _MAIN_UART_H_
#define _MAIN_UART_H_



#include <string.h>
#include <avr/interrupt.h>

#define BAUDRATE				9600 // Скорость обмена данными
#define UART_BUFF_SIZE			32
#define UART_BUFF_MASK			UART_BUFF_SIZE - 1




//--- FUNCTIONS -----------------------------------------------------------------------------
void uart_init(void);
void uart_send(const uint8_t data);
void str_uart_send(char *string);
void uart_send_buff(uint8_t* buff, const uint8_t len);
uint8_t uart_receive(void);
uint8_t uart_isReadAvailable(void);
uint8_t uart_readData(uint8_t* buff);
void uart_sendHex(const uint8_t hexdig);
void uart_newline(void);

#endif // _MAIN_UART_H_
