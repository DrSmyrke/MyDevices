#include <avr/io.h>
//#include <util/delay.h>
#define SCLK 7
#define IO 6
#define RESET 5

#define DDR_RTC DDRD
#define PORT_RTC PORTD
#define PIN_RTC PIND

#define AM 0
#define PM 0b00100000

#define H12 0b10000000
#define H24 0

//посылаем команду или байт данных в часы
void ds1302_write(unsigned char cmd){
	DDR_RTC |= (1<<RESET) | (1<<SCLK)|(1<<IO);
	PORT_RTC |= (1<<RESET);	//RESET=1
	delay(400);
	//_delay_us(4);
	DDR_RTC |= (1<<IO);	//выход
	unsigned char i;
	for(i=0; i<8; i++){
		if((cmd&(1<<i)) == 1<<i){PORT_RTC |= (1<<IO);}else{PORT_RTC &= ~(1<<IO);}
		PORT_RTC |= (1<<SCLK);
		delay(100);
		//_delay_us(1);
		PORT_RTC &= ~(1<<IO);
		PORT_RTC &= ~(1<<SCLK);
	}
}
unsigned char ds1302_read(){
	unsigned char readbyte=0;
	DDR_RTC &= ~(1<<IO);
	unsigned char i;
	for(i=0;i<8;i++){
		PORT_RTC |= 1<<SCLK;
		if((PIN_RTC & (1<<IO))==0){readbyte &= ~(1<<i);}else{readbyte |= 1<<i;}
		delay(1000);
		//_delay_us(10);
		PORT_RTC &= ~(1<<SCLK);
		delay(200);
		//_delay_us(2);
	}
	PORT_RTC &= ~(1<<RESET);
	delay(400);
	//_delay_us(4);
	return readbyte;
}
unsigned char ReceiveSeconds(){
	unsigned char tmp, tmp10;
	ds1302_write(0x81);
	tmp=ds1302_read();
	//отделяем десятки и единицы отдельно
	tmp10 = ((tmp>>4) & 0b00000111);
	tmp &= 0x0F;
	//delay(10);
	return 10*tmp10+tmp;
}
unsigned char ReceiveMinutes(){
	unsigned char tmp, tmp10;
	ds1302_write(0x83);
	tmp=ds1302_read();
	//отделяем десятки и единицы отдельно
	tmp10 = ((tmp>>4) & 0b00000111);
	tmp &= 0x0F;
	//delay(10);
	return 10*tmp10+tmp;
}
unsigned char ReceiveHours(){
	unsigned char tmp, tmp10;
	ds1302_write(0x85);
	tmp=ds1302_read();
	AMPM = (tmp & 0b00100000);
	H12_24 = (tmp & 0b10000000);
	if (H12_24 == H12){tmp = tmp & 0b00011111;}else{tmp = tmp & 0b00111111;}
	//отделяем десятки и единицы отдельно
	tmp10 = ((tmp>>4) & 0b00000111);
	tmp &= 0x0F;
	//hour &= 0b00011111;
	//delay(10);
	//return hour;
	return 10*tmp10+tmp;
}
unsigned char ReceiveDay(){
	unsigned char tmp,tmp10;
	ds1302_write(0x87);
	tmp=ds1302_read();
	//отделяем десятки и единицы отдельно
	tmp10 = ((tmp>>4) & 0b00000111);
	tmp &= 0x0F;
	//delay(10);
	return 10*tmp10+tmp;
}
unsigned char ReceiveMonth(){
	unsigned char tmp,tmp10;
	ds1302_write(0x89);
	tmp=ds1302_read();
	//отделяем десятки и единицы отдельно
	tmp10 = ((tmp>>4) & 0b00000111);
	tmp &= 0x0F;
	//delay(10);
	return 10*tmp10+tmp;
}
unsigned char ReceiveYear(){
	unsigned char tmp,tmp10;
	ds1302_write(0x8D);
	tmp=ds1302_read();
	//отделяем десятки и единицы отдельно
	tmp10 = ((tmp>>4) & 0x0F);
	tmp &= 0x0F;
	//delay(10);
	return 10*tmp10+tmp;
}
unsigned char ReceiveDayOfWeek(){
	ds1302_write(0x8B);
	return ds1302_read();
}
uint8_t DS1302_Bin8_To_BCD(uint8_t data){
	uint8_t nibh;
	uint8_t nibl;
	nibh=data/10;
	nibl=data-(nibh*10);
	return((nibh<<4)|nibl);
}
//вызываем после записи байта данных в часы
void ds1302_end_write_data(){PORT_RTC &= ~(1<<RESET);}
ds1302_ClrWP(){
	ds1302_write(0x8E);
	ds1302_write(0x00);
	ds1302_end_write_data();
}
void ds1302_StartTime(){
	ds1302_ClrWP();
	unsigned char tmp;
	//set seconds and bit CH
	ds1302_write(0x81);
	tmp=ds1302_read();	
	tmp&=~(1<<7);//bit CH is 7- сбрасываем
	ds1302_write(0x80);
	ds1302_write(tmp);
	ds1302_end_write_data();
}
void ds1302_SetNewTime(unsigned char seconds,unsigned char minute,unsigned char hour){
	unsigned char temp;
	uint8_t tmp;
	temp=seconds/10;seconds%=10;temp<<=4;seconds |= temp;
	ds1302_write(0x80);ds1302_write(seconds);ds1302_end_write_data();
	temp=minute/10;minute%=10;temp<<=4;minute |= temp;
	ds1302_write(0x82);ds1302_write(minute);ds1302_end_write_data();
	temp=hour/10;hour%=10;temp<<=4;hour |= temp;
	tmp = (hour | AMPM | H12_24);
	ds1302_write(0x84);ds1302_write(tmp);ds1302_end_write_data();
	ds1302_StartTime();
}
void ds1302_SetNewDate(unsigned char day,unsigned char month,unsigned char year,unsigned char dayofweek){
	unsigned char temp;
	temp=day/10;day%=10;temp<<=4;day |= temp;
	ds1302_write(0x86);ds1302_write(day);ds1302_end_write_data();
	temp=month/10;month%=10;temp<<=4;month |= temp;
	ds1302_write(0x88);ds1302_write(month);ds1302_end_write_data();
	//temp=hour/10;hour%=10;temp<<=4;hour |= temp;
	ds1302_write(0x8A);ds1302_write(dayofweek);ds1302_end_write_data();
	temp=year/10;year%=10;temp<<=4;year |= temp;
	ds1302_write(0x8C);ds1302_write(year);ds1302_end_write_data();
	ds1302_StartTime();
}
/*
uint8_t DS1302_BCD_To_Bin8(uint8_t data){
unsigned char result;		
	result = ((data>>4) & 0b00000111);
	data &= 0x0F;
	data = data + result*10;
	return data;
}
*/