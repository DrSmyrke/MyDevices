void sleep_init(void)
{
	// Init Power-Down mode
	setZero(MCUCR,SM2);
	setPlus(MCUCR,SM1);
	setZero(MCUCR,SM0);
}

void sleep_enable(void)
{
	MCUCR |= (1<<SE);
}

void sleep_disabled(void)
{
	MCUCR &=~ (1<<SE);
}

void sleep(void)
{
	asm("sleep");
}
