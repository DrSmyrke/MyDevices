/*

	Commands to ps/2 master
cmd		response	info	
0xEE	0xEE		echo
0xFF	0xAA		reset device
0xF5	0xFA		off device
0xF4	0xFA		on device
0xFE	-			resend byte
0xED	0xFA		change leds state

	Change leds state
	send 0xED,	0b0000 0001
				        |||_ scroll lock LED bit (on)
				        ||__ Num lock LED bit (off)
				        |___ Caps lock LED bit (off)

	Change leds state dump
	0xED			send to master
	0xFA			response
	0b00000001		set scroll lock LED on
	0xFA			response

*/

#include <avr/io.h>
#include <avr/interrupt.h>

#define PS2_CLK_DDR DDRD
#define PS2_DAT_DDR DDRD
#define PS2_CLK_PORT PORTD
#define PS2_DAT_PORT PORTD
#define PS2_CLK_PIN 2
#define PS2_DAT_PIN 4
#define PS2_CLK_READ PIND&(1<<PS2_CLK_PIN)
#define PS2_DAT_READ PIND&(1<<PS2_DAT_PIN)

volatile unsigned char action_int = 11,temporary_data_keyboard,parity_bit_keyboard; 
volatile unsigned char RX_TX_keyboard, keyboard_data;


//****************************//
// отправляем данные
//****************************//
//****************************//
void keypad_output_data( register unsigned char Temp) 
{
	GICR &=~(1<<INT0);    // запрещаем работу прерыванию INT       	  	
	PS2_DAT_DDR |=(1<<PS2_DAT_PIN);     // на выход  
	PS2_CLK_DDR |=(1<<PS2_CLK_PIN);	   // на выход
	PS2_DAT_PORT &=~ (1<<PS2_DAT_PIN);  // низкий уровень
	PS2_CLK_PORT &=~ (1<<PS2_CLK_PIN);  // низкий уровень
	RX_TX_keyboard |=(1<<7);        // переводим на передачю данных       
	temporary_data_keyboard = Temp; // сохраняем передаваемые данные
	parity_bit_keyboard = 0;
	action_int = 0;
	PS2_CLK_PORT |= (1<<PS2_CLK_PIN);  
	PS2_CLK_DDR &=~(1<<PS2_CLK_PIN); 
	GIFR |= (1<<INT0);   // сброс флага прерывания INT		 	
	GICR |= (1<<INT0);    // разрешаем работу прерыванию INT
}
 //----------------//
void keypad_output (void)
{
	switch ( action_int ){	  	   	 
		case 0 ... 7: // передаем по битно данные
			if ( temporary_data_keyboard & (1 << action_int)){
				PS2_DAT_PORT |=(1<<PS2_DAT_PIN);
				parity_bit_keyboard ++; // подсчитываем количество для бита четности
			}else{ PS2_DAT_PORT &=~(1<<PS2_DAT_PIN); }
		break;
		case 8:
			// передаем бит четности
			if (parity_bit_keyboard & (1 << 0)) 
			       PS2_DAT_PORT &=~(1<<PS2_DAT_PIN);
				else
				 {
		case 9:	// стоповый бит 	   	        
				PS2_DAT_PORT |=(1<<PS2_DAT_PIN); 
				PS2_DAT_DDR &=~(1<<PS2_DAT_PIN); 
			}
		break;
		case 10:	  				  
			parity_bit_keyboard = 0;
			temporary_data_keyboard = 0;   
			RX_TX_keyboard &=~(1<<7);      // принимаем ответ от клавиатуры
		break;   	   		   	   	 
	}
	action_int ++;  
} 
// Прием данных с порта
void ps2_inputKey(uint8_t key)
{
	static uint8_t keyUp = 0, LED_number = 2;
	uart_send(key);
	switch ( key ){
		case 0xaa:
			keypad_output_data(0xED);
			keypad_output_data(2);
			LED_number = 2;
		break;
		case 0x77:	// код кнопки на клавиатуре "Num/Lock"
			if ( !keyUp ){
				// ожидаем нажатие кнопки 				  
				break;	
			}
			keyUp = 0;
			// команда -Установить светодиоды состояния 0xED
			keypad_output_data(0xED);
		break;
		case 0xfa:   // Подтверждение о принятии данных либо команды
			LED_number++;
			if (LED_number == 1) keypad_output_data(1);    // светодиод "Scroll Lock"	
			if (LED_number == 2) keypad_output_data(4);    // светодиод "Caps lock"	
			if (LED_number== 3){
				keypad_output_data(2);	// светодиод "Num Lock"	
				LED_number = 0; 
			}
		break;
		// кнопка отжата
		case 0xf0: keyUp = 1; break;
	}
}
void ps2_init(void)
{
	PS2_CLK_DDR &=~(1<<PS2_CLK_PIN);
	PS2_DAT_DDR &=~(1<<PS2_DAT_PIN);
	PS2_CLK_PORT |= (1<<PS2_CLK_PIN);
	PS2_DAT_PORT |= (1<<PS2_DAT_PIN);
	//настраиваем на Задний фронт на INT0
	MCUCR |=(1<<ISC01);
	MCUCR &=~(1<<ISC00);
	//разрешаем внешнее прерывание INT0 
	GICR |= (1<<INT0);
}
//****************************//
//****************************//
// принимаем данные
//****************************//   
//****************************//
void keypad_input (void)
{
	switch ( action_int ){
		case 11:
			// определение стартового бита
			if ( !(PS2_DAT_READ) ) action_int = 0;
		break;
		case 0 ... 7:
			// принимаем 8 бит	данных
			if ( PS2_DAT_READ ){
				temporary_data_keyboard |= (1<<action_int);
				// данные для проверки чётности
				parity_bit_keyboard ++;
			}else{
				temporary_data_keyboard &=~(1<<action_int);
			}
			// счёт принятых бит
			action_int++;
		break;
		case 8:
			// проверяем на правельность принятых данных исходя от бита чётности
			// принимаем бит чётности от клавиатуры
			if( PS2_DAT_READ ){
				// проверяем четное или не четное число
				action_int = (parity_bit_keyboard & (1<<0)) ? 11 : 9 ;
			}else{
				// проверяем четное или не четное число
				action_int = (parity_bit_keyboard & (1<<0)) ? 9 : 11 ;
			}
			parity_bit_keyboard = 0;
			break;
		case 9:
			// байт принят
			// стоповый бит (всегда высокий уровень)
			// сохраняем полученое значение
			if ( PS2_DAT_READ ) keyboard_data = temporary_data_keyboard;
			action_int = 11;
		break;
	} 
}
ISR(INT0_vect){
	if ( RX_TX_keyboard & (1<<7)){
		keypad_output(); // отправляем данные на клавиатуру	
	}else{
		keypad_input(); //принимаем данные с клавиатуры	
	}	      
}
