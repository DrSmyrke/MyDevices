#define BAUDRATE 9600 // Скорость обмена данными

#include <string.h>
#include <avr/interrupt.h>

#define UART_BUFF_SIZE			32
#define UART_BUFF_MASK			UART_BUFF_SIZE - 1

volatile uint8_t uart_rx_buff[UART_BUFF_SIZE];
volatile uint8_t uart_rx_rIndx = 0;
volatile uint8_t uart_rx_wIndx = 0;

void uart_init(){
	//Параметры соединения: 8 бит данные, 1 стоповый бит, нет контроля четности
	//USART Приемник: Включен
	//USART Передатчик: Включен
	//USART Режим: Асинхронный
	//USART Скорость обмена: 9600
    //UCSRA=0x00;
    UCSRB=0xd8;
	UCSRC=0x86;
	UBRRH=0x00;
    UBRRL=0x33;

	UCSRB = ( 1 << TXEN ) | ( 1 << RXEN ) | ( 1 << RXCIE ) | ( 1 << TXCIE ) | ( 0 << UDRIE );
	UCSRC = (1 << URSEL)| // Для доступа к регистру UCSRC выставляем бит URSEL
	(1 << UCSZ1)|(1 << UCSZ0); // Размер посылки в кадре 8 бит
}

// Функция передачи данных по USART
void uart_send( const uint8_t data )
{
	while(!( UCSRA & (1 << UDRE)));   // Ожидаем когда очистится буфер передачи
	UDR = data; // Помещаем данные в буфер, начинаем передачу
}

 // Функция передачи строки по USART
void str_uart_send(char *string)
{
	while(*string != '\0'){
		uart_send(*string);
		string++;
	}
}

void uart_send_buff( uint8_t* buff, const uint8_t len )
{
	uint8_t i;
	for( i = 0; i < len; i++ ){
		uart_send( buff[i] );
	}
}

uint8_t uart_receive(void) {
    while(!(UCSRA & (1<<RXC)) );
    return UDR;
}

uint8_t uart_isReadAvailable()
{
	return ( uart_rx_wIndx != uart_rx_rIndx ) ? 0x01 : 0x00;
}

uint8_t uart_readData(uint8_t* buff)
{
	uint8_t ch = 0;
	
	while( uart_rx_wIndx != uart_rx_rIndx ){
		buff[ch++] = uart_rx_buff[ uart_rx_rIndx++ & UART_BUFF_MASK ];
	}
	
	buff[ch] = 0x00;
	
	return ch;
}

void uart_sendHex( const uint8_t hexdig )
{
	uart_send( (hexdig >> 4) + (((hexdig >> 4) >= 10) ? ('A' - 10) : '0')) ;
	uart_send( (hexdig & 0x0F) + (((hexdig & 0x0F) >= 10) ? ('A' - 10) : '0') );
}

void uart_newline()
{
	uart_send( '\r' );
	uart_send( '\n' );
}

// Прерывание по окончанию приема данных по USART
ISR(USART_RXC_vect)
{
	uint8_t sym = uart_receive();
	uart_rx_buff[ uart_rx_wIndx++ & UART_BUFF_MASK ] = sym;
}

// Отправка данных
ISR(USART_TXC_vect)
{
	
}