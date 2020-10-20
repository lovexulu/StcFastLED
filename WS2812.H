#ifndef __WS2812_H__
#define __WS2812_H__
#include "stc15.h"
#include "intrins.h"
#define uint unsigned int
#define uchar unsigned char


sbit n0=P1^0;

void delay(uint x);
void st1();
void st0();
void stop();







#endif