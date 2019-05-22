#include <avr/io.h>
#include <util/delay.h>

#define TM1637_CLK_PIN 		0
#define TM1637_CLK_PORT 	PORTB
#define TM1637_CLK_DDR		DDRB

#define TM1637_DIO_PIN 		5
#define TM1637_DIO_PORT 	PORTD
#define TM1637_DIO_DDR		DDRD

	//************definitions for TM1637*********************
#define ADDR_AUTO  0x40
#define ADDR_FIXED 0x44
#define STARTADDR  0xc0 
	/**** definitions for the clock point of the 4-Digit Display *******/
#define POINT_ON   1
#define POINT_OFF  0
//--------------------------------------------------------//
//Special characters index of tube table
#define INDEX_NEGATIVE_SIGN	16
#define INDEX_BLANK			17
/************definitions for type of the 4-Digit Display*********************/
#define D4036B 0
#define D4056A 1
		
		uint8_t Cmd_DispCtrl;
		uint8_t _PointFlag; 	//_PointFlag=1:the clock point on
		uint8_t _DispType;
		int8_t coding(int8_t DispData);
		uint8_t DecPoint;

static uint8_t TubeTab[] = {0x3f,0x06,0x5b,0x4f,// знакогенератор
                           0x66,0x6d,0x7d,0x07,
                           0x7f,0x6f,0x77,0x7c,
                           0x39,0x5e,0x79,0x71,
                           0x40,0x00};//0~9,A,b,C,d,E,F,"-"," "  



void TM1637_writeByte(int8_t wr_data)// служебная функция записи данных по протоколу I2C, с подтверждением (ACK)
{
  uint8_t i;
    for(i=0;i<8;i++)        
  {
   TM1637_CLK_PORT &= ~(1<<TM1637_CLK_PIN);
    if(wr_data & 0x01)
	{ TM1637_DIO_PORT |= 1<<TM1637_DIO_PIN;}
    else {TM1637_DIO_PORT &= ~(1<<TM1637_DIO_PIN);}
	_delay_us(3);
    wr_data = wr_data>>1;      
    TM1637_CLK_PORT |= 1<<TM1637_CLK_PIN;
	_delay_us(3);  
  }  
 
  TM1637_CLK_PORT &= ~(1<<TM1637_CLK_PIN);
  _delay_us(5);
  TM1637_DIO_DDR &= ~(1<<TM1637_DIO_PIN);
  while((PINC & (1<<TM1637_DIO_PIN))); 
  TM1637_DIO_DDR |= (1<<TM1637_DIO_PIN);
  TM1637_CLK_PORT |= 1<<TM1637_CLK_PIN;
  _delay_us(2);
  TM1637_CLK_PORT &= ~(1<<TM1637_CLK_PIN);  
}

void TM1637_start(void) // просто функция "старт" для протокола I2C
{
   TM1637_CLK_PORT |= 1<<TM1637_CLK_PIN; 
   TM1637_DIO_PORT |= 1<<TM1637_DIO_PIN;
 _delay_us(2);
  TM1637_DIO_PORT &= ~(1<<TM1637_DIO_PIN); 
} 

void TM1637_stop(void) // просто функция "стоп" для протокола I2C
{
  TM1637_CLK_PORT &= ~(1<<TM1637_CLK_PIN);
 _delay_us(2);
  TM1637_DIO_PORT &= ~(1<<TM1637_DIO_PIN);
_delay_us(2);
  TM1637_CLK_PORT |= 1<<TM1637_CLK_PIN;;
_delay_us(2);
  TM1637_DIO_PORT |= 1<<TM1637_DIO_PIN;
}

void TM1637_coding_all(uint8_t DispData[])//шифратор всех знакомест
{
	uint8_t PointData = (_PointFlag == POINT_ON) ? 0x80 : 0;
	uint8_t i;
	for(i = 0;i < 4;i ++){
		DispData[i] = (DispData[i] == 0x7f) ? 0x00 + PointData : TubeTab[DispData[i]] + PointData;
		DispData[i] += 0x80;
	}
	if((_DispType == D4056A)&&(DecPoint != 3)){
		DispData[DecPoint] += 0x80;
		DecPoint = 3;
	}
}
int8_t TM1637_coding(uint8_t DispData)// шифратор
{
	uint8_t PointData = (_PointFlag == POINT_ON) ? 0x80 : 0;
	DispData = (DispData == 0x7f) ? 0x00 : TubeTab[DispData] + PointData;
	return DispData;
}
// полезняшка! отображает содержимое массива прямо на экране.
//не забываем, что можем отображать только числа от 0x00 до 0x0F или по-русски от 0 до 15
void TM1637_display_all(uint8_t DispData[])
{
  uint8_t SegData[4];
  uint8_t i;
  for(i = 0;i < 4;i ++){ SegData[i] = DispData[i]; }
  TM1637_coding_all(SegData);
  TM1637_start();
  TM1637_writeByte(ADDR_AUTO);
  TM1637_stop();
  TM1637_start();
  TM1637_writeByte(STARTADDR);
  for(i=0;i < 4;i ++){ TM1637_writeByte(SegData[i]); }
  TM1637_stop();
  TM1637_start();
  TM1637_writeByte(Cmd_DispCtrl);
  TM1637_stop();
}
// отображает один символ (от 0 до 15)в определенном месте  (от 0 до 3)
void TM1637_display(uint8_t BitAddr,int8_t DispData)
{
  int8_t SegData;
  SegData = TM1637_coding(DispData);
  TM1637_start();
  TM1637_writeByte(ADDR_FIXED);
  TM1637_stop();
  TM1637_start();
  TM1637_writeByte(BitAddr|STARTADDR);
  TM1637_writeByte(SegData);
  TM1637_stop();
  TM1637_start();
  TM1637_writeByte(Cmd_DispCtrl);
  TM1637_stop();
}
// функция с минимальными изменениями скопированная с ардуиновской библиотеки. выводит целые числа от -999 до 9999
void TM1637_display_int_decimal(int16_t Decimal, const uint8_t BlankingFlag)
{
	uint8_t temp[4];
	if((Decimal > 9999)||(Decimal < -999)) return;
	if(Decimal < 0){
		temp[0] = INDEX_NEGATIVE_SIGN;
		Decimal = (Decimal & 0x7fff);
		temp[1] = Decimal/100;
		Decimal %= 100;
		temp[2] = Decimal / 10;
		temp[3] = Decimal % 10;
		if(BlankingFlag){
			if(temp[1] == 0){
				temp[1] = INDEX_BLANK;
				if(temp[2] == 0) temp[2] = INDEX_BLANK;
			}
		}
	}else{
		temp[0] = Decimal/1000;
		Decimal %= 1000;
		temp[1] = Decimal/100;
		Decimal %= 100;
		temp[2] = Decimal / 10;
		temp[3] = Decimal % 10;
		if(BlankingFlag){
			if(temp[0] == 0){
				temp[0] = INDEX_BLANK;
				if(temp[1] == 0){
					temp[1] = INDEX_BLANK;
					if(temp[2] == 0) temp[2] = INDEX_BLANK;
				}
			}
		}
	}
	TM1637_display_all(temp);
} 

void TM1637_display_float_decimal(double Decimal) // функция с минимальными изменениями скопированная с ардуиновской библиотеки. 
{												// выводит числа с десятичной точкой от -999 до 9999
  int16_t temp;
  if(Decimal > 9999)return;
  else if(Decimal < -999)return;
  uint8_t i = 3;
  if(Decimal > 0)
  {
	for( ;i > 0; i --)
	{
	  if(Decimal < 1000)Decimal *= 10;
	  else break;
	}
	temp = (int)Decimal;
	if((Decimal - temp)>0.5)temp++;
  }
   else
  {
	for( ;i > 1; i --)
	{
	  if(Decimal > -100)Decimal *= 10;
	  else break;
	}
	temp = (int)Decimal;
	if((temp - Decimal)>0.5)temp--;
  }
  DecPoint = i;
  TM1637_display_int_decimal(temp,0);
}

void TM1637_clearDisplay(void) // чистит дисплей
{
	TM1637_display(0x00,0x7f);
	TM1637_display(0x01,0x7f);
	TM1637_display(0x02,0x7f);
	TM1637_display(0x03,0x7f);  
}
void TM1637_init(uint8_t DispType)// инициализирует переменные, назначает порт и пины для связи с дисплеем, а потом чистит дисплей
{
	TM1637_CLK_DDR |= (1<<TM1637_CLK_PIN);
	TM1637_DIO_DDR |= (1<<TM1637_DIO_PIN);

	_DispType = DispType;
	DecPoint = 3;
	TM1637_clearDisplay();
}
void TM1637_setBrightness(uint8_t brightness)// для установки яркости нужна
{
	if(brightness > 7 || brightness < 0) brightness = 0;
	Cmd_DispCtrl = 0x88 + brightness;
}
void TM1637_point(uint8_t PointFlag)//не знаю зачем скопировал, пусть будет
{
	_PointFlag = PointFlag;
}
/*
	TM1637_set(0-7);	установка яркости
*/
