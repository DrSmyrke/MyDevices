#ifndef __MAIN_TIMER_H_
#define __MAIN_TIMER_H_

#include "main.h"

os_timer_t myTimer;

void timerCallback(void *pArg)
{
	flags.timer = 1;
}

void timer_init(void)
{
	os_timer_setfn( &myTimer, timerCallback, NULL );
	os_timer_arm( &myTimer, 1000, true );
}

#endif //__MAIN_TIMER_H_

