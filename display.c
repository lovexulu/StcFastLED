#include "ws2812.h"

void delay(uint x)
{
	while(x--);
}

void st1()
{
	n0=1;
	delay(1);
	_nop_();
	_nop_();
	n0=0;	
}
void st0()
{
	n0=1;
	_nop_();
	_nop_();
	n0=0;
	delay(1);
	
}
void stop()
{
	delay(800);
}