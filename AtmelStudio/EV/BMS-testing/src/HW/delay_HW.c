/*
 * delay_HW.c
 *
 * Created: 2019-12-16 19:16:18
 *  Author: Måns Lindeberg
 */ 

#include <stdint.h>
#include "delay_HW.h"
void delay_u(uint16_t micro)
{
	delay_cycles_us(micro);
}

void delay_m(uint16_t milli)
{
	delay_cycles_ms(milli);
}
