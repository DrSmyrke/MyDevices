#include <avr/io.h>
#include <avr/interrupt.h>
#define PS2_CLK_DDR DDRD
#define PS2_DAT_DDR DDRB
#define PS2_CLK_PORT PORTD
#define PS2_DAT_PORT PORTB
#define PS2_CLK_PIN 2
#define PS2_DAT_PIN 4
#define PS2_CLK_READ PIND&(1<<PS2_CLK_PIN)
#define PS2_DAT_READ PINB&(1<<PS2_DAT_PIN)

volatile unsigned char action_int = 11,temporary_data_keyboard,parity_bit_keyboard; 
volatile unsigned char keyboard_data,bit_action;  
//********************************//
void keypad_input ()
{
  switch ( action_int ){
    case 11:
      if (!(PS2_DAT_READ))      // ����������� ���������� ����
        action_int = 0;
      break;
    case 0 ... 7:                      // ��������� 8 ��� ������
      if (PS2_DAT_READ){
        temporary_data_keyboard |=(1<<action_int);
        parity_bit_keyboard ++; // ������ ��� �������� �������� 
      }else{
        temporary_data_keyboard &=~(1<<action_int);
      }
      action_int++;            // ���� �������� ���
      break;
    case 8:                        // ��������� �� ������������ �������� ������ ������ �� ���� ��������
      if  (PS2_DAT_READ){
        if (parity_bit_keyboard & (1<<0)){action_int = 11;}else{action_int = 9;}
      }else{
        if (parity_bit_keyboard & (1<<0)) // ��������� ������ ��� �� ������ �����
           action_int = 9;
         else
           action_int = 11; // ��� �������� �� ����������
      }
        parity_bit_keyboard = 0;
        break;
    case 9:   
        if (PS2_DAT_READ){
        keyboard_data = temporary_data_keyboard; // ��������� ���������� ��������
      }
        action_int = 11;
        break;
    } 
 }
//ISR(INT0_vect)
//{
//	keypad_input();
//}

//********************************//
void ps2_init()
{
	PS2_CLK_DDR &=~(1<<PS2_CLK_PIN);  
	PS2_DAT_DDR &=~(1<<PS2_DAT_PIN);
	PS2_CLK_PORT |=(1<<PS2_CLK_PIN);
	PS2_DAT_PORT |=(1<<PS2_DAT_PIN); 
	//INT0 PORTD2 CLK
 attachInterrupt(0,keypad_input,FALLING);
	//GICR |=(1<<INT0);
 
	//MCUCR |=(1<<ISC01);
	//MCUCR &=~(1<<ISC00);
	//asm("sei");
}
void ps2_read()
{
	if(!(PS2_CLK_READ)) keypad_input();
}
