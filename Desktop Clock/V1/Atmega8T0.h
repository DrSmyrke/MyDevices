ISR( TIMER0_OVF_vect ){
	//TCNT0=0x83; //выставляем начальное значение TCNT0
	if(bgl<30){
		bgl++;
		setPlus(PORTB,0);
	}else{
		setZero(PORTB,0);
		if(bgl<190 && menu){
			bgl++;
		}else{
			if(menu){lcd_clear();delay(1000);menu=0;mode=0;}
		}
	}
	if(ch<3){ch++;}else{ch=0;}
	if(ch==2 && !menu){
		unsigned char atmp;
		for(alarmNumB=0;alarmNumB<=9;alarmNumB+=3){
			atmp=eeprom_read(alarmNumB);
			if(CheckBit(atmp,0)){	//ON
				if(eeprom_read(alarmNumB+1)==Hour && eeprom_read(alarmNumB+2)==Min){
					if(DayOfWeek==1 && CheckBit(atmp,1)){play();}
					if(DayOfWeek==2 && CheckBit(atmp,2)){play();}
					if(DayOfWeek==3 && CheckBit(atmp,3)){play();}
					if(DayOfWeek==4 && CheckBit(atmp,4)){play();}
					if(DayOfWeek==5 && CheckBit(atmp,5)){play();}
					if(DayOfWeek==6 && CheckBit(atmp,6)){play();}
					if(DayOfWeek==7 && CheckBit(atmp,7)){play();}
				}
			}
		}
		if(alarmPort){alarmPort=0;setZero(PORTB,2);}
	}
	if(ch==1){
		if(!menu){
			if(Month>=5 && Month<=9 && Hour>=22 && Hour<=23){bgl=0;}
			if(Month>=10 && Month<=12 && Hour>=21 && Hour<=23){bgl=0;}
			if(Month>=1 && Month<=4 && Hour>=21 && Hour<=23){bgl=0;}
			Hour=ReceiveHours();
			Min=ReceiveMinutes();
			Sec=ReceiveSeconds();
			Day=ReceiveDay();
			Month=ReceiveMonth();
			Year=ReceiveYear();
			DayOfWeek=ReceiveDayOfWeek();
			sprintf(buff, "%02i.%02i.20%2i ",Day,Month,Year);
			lcd_goto_xy(1,6);lcd_str(buff);
			if(Hour<10){
				lcd_chrNum(0,1,2);
				lcd_chrNum(Hour,17,2);
			}else{
				lcd_chrNum(Hour/10,1,2);
				lcd_chrNum(Hour-((Hour/10)*10),17,2);
			}
			lcd_goto_xy_exact(33,3);
			lcd_str(":");
			if(Min<10){
				lcd_chrNum(0,39,2);
				lcd_chrNum(Min,55,2);
			}else{
				lcd_chrNum(Min/10,39,2);
				lcd_chrNum(Min-((Min/10)*10),55,2);
			}
			lcd_goto_xy_exact(70,3);
			lcd_str(":");
			lcd_goto_xy_exact(74,3);
			sprintf(buff, "%02i",Sec);
			lcd_str(buff);
			lcd_chrWD(DayOfWeek,74,6);
		}
	}
}
void t0_init(){
	//TCCR0|=(1<<CS01)|(1<<CS00);//предделитель/64
	TCCR0|=(1<<CS02)|(1<<CS00);//предделитель/1024
	TIMSK|=(1<<TOIE0); // Разрешаем прерывание по переполнению Т0
	//SREG|=(1<<I);//глобальное разрешение прерывний
	//TCNT0=0x83;        // выставляем начальное значение TCNT0
	// Global enable interrupts
	sei();
}