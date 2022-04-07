#ifndef _MAIN_H_
#define _MAIN_H_

#include <avr/io.h>

#include <stdlib.h>
#include <light_ws2812.h>

#include <Atmega8T0.h>
#include <adc.h>
#include <INT0.h>
#include <sleep.h>

#ifdef __UART
	#include "uart.h"
#endif

#endif //_MAIN_H_
