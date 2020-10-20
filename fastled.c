#include "stc15.h"
#include "ws2812.h"
uchar xdata rgb[60][3];//led数组
uchar sendrgb=0;//预发送rgb变量
uchar h=0,l=0,o=0;
uchar in=0;
void delayms(int x)
{
	int i,j;
	for(i=0;i<x;i++)
	for(j=0;j<1000;j++);
}

void UartInit(void)		//115200bps@33.1776MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x01;		//串口1选择定时器2为波特率发生器
	AUXR |= 0x04;		//定时器2时钟为Fosc,即1T
	T2L = 0xB8;		//设定定时初值
	T2H = 0xFF;		//设定定时初值
	AUXR |= 0x10;		//启动定时器2
	ES = 1;              //使能串口1中断
}
void initrgb(uchar st,uchar sp,uchar rg0,uchar rg1,uchar rg2)//起始位，结束位，rgb1，2，3(初始化数组）
{
uchar i,j;
	for(i=st;i<sp;i++)
	{		
		for(j=0;j<3;j++)
		{
			switch(j)
			{
				case 0:
			rgb[i][j]=rg0;
				break;
				case 1:
			rgb[i][j]=rg1;
				break;
				case 2:
			rgb[i][j]=rg2;
				break;
			}
		}
	}	
}

void display()     //刷新rebled输出函数
{
uchar i,j,k;
	for(i=0;i<60;i++)
	{
		for(j=0;j<3;j++)
		{
			sendrgb=rgb[i][j];
			for(k=0;k<8;k++)
			{
				if(sendrgb&0x80)
					st1();
				else
					st0();
				sendrgb<<=1;
			}
		}
	}
	stop();
}

void main()
{
UartInit();//初始化串口
initrgb(0,60,50,0,0);	//测试绿
display();
delayms(1000);
initrgb(0,60,0,50,0);	//测试红
display();
delayms(1000);
initrgb(0,60,0,0,50);	//测试蓝
display();
delayms(1000);
initrgb(0,60,0,0,0);	//清屏
display();
delayms(1000);	
EA = 1;   //开启中断，可接收数据
	
	while(1)
		{
		if(EA==0)//当中断关闭，表示一帧数据接收完毕
			{
				display();//将当前数据刷新
				EA=1;//开启中断，接收下一帧数据
			}
		}	
}

void Uart() interrupt 4   //串口中断
{
	uchar dat=0;
    if (RI)
    {
		dat=SBUF;
        RI = 0;                 //清除RI位
        if(dat==0x41&&in==0)
		{
			in=1;					//判断数据0位（A）
			return;
		}
		if(dat==0x64&&in==1)		//判断数据1位（d）
		{
			in=2;
			return;
		}
		else if(dat==0x61&&in==2)	//判断数据2位（a）
		{
			in=3;
			return;
		}
		else if(in==3)				//读取高位LED个数
		{
			h=dat;
			in++;
			return;
		}
		else if(in==4)				//读取低位LED个数
		{
			l=dat;
			in++;
			return;
		}
		else if(in==5)				//读取校验值
		{
			o=dat;
			if(o!=(h^l^0x55)) in=0;		//校验
			else in++;
			return;
		}
		else if(in>=6)				//进入数据读取阶段
		{
			rgb[(in-6)/3][(in-6)%3]=dat;//将数据写入数组
			if((in-6)/3==l)				//根据发来的led个数确定数据是否接收完毕
			{
				in=0;
				EA=0;
				return;
			}
			else in++;
			return;
		}
			else  			//如果in小于6，且中途数据中断。则重新开始。
			{
				in=0;
				return;
			}
	}
}