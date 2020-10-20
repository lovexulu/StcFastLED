#include "stc15.h"
#include "ws2812.h"
uchar xdata rgb[60][3];//led����
uchar sendrgb=0;//Ԥ����rgb����
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
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x01;		//����1ѡ��ʱ��2Ϊ�����ʷ�����
	AUXR |= 0x04;		//��ʱ��2ʱ��ΪFosc,��1T
	T2L = 0xB8;		//�趨��ʱ��ֵ
	T2H = 0xFF;		//�趨��ʱ��ֵ
	AUXR |= 0x10;		//������ʱ��2
	ES = 1;              //ʹ�ܴ���1�ж�
}
void initrgb(uchar st,uchar sp,uchar rg0,uchar rg1,uchar rg2)//��ʼλ������λ��rgb1��2��3(��ʼ�����飩
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

void display()     //ˢ��rebled�������
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
UartInit();//��ʼ������
initrgb(0,60,50,0,0);	//������
display();
delayms(1000);
initrgb(0,60,0,50,0);	//���Ժ�
display();
delayms(1000);
initrgb(0,60,0,0,50);	//������
display();
delayms(1000);
initrgb(0,60,0,0,0);	//����
display();
delayms(1000);	
EA = 1;   //�����жϣ��ɽ�������
	
	while(1)
		{
		if(EA==0)//���жϹرգ���ʾһ֡���ݽ������
			{
				display();//����ǰ����ˢ��
				EA=1;//�����жϣ�������һ֡����
			}
		}	
}

void Uart() interrupt 4   //�����ж�
{
	uchar dat=0;
    if (RI)
    {
		dat=SBUF;
        RI = 0;                 //���RIλ
        if(dat==0x41&&in==0)
		{
			in=1;					//�ж�����0λ��A��
			return;
		}
		if(dat==0x64&&in==1)		//�ж�����1λ��d��
		{
			in=2;
			return;
		}
		else if(dat==0x61&&in==2)	//�ж�����2λ��a��
		{
			in=3;
			return;
		}
		else if(in==3)				//��ȡ��λLED����
		{
			h=dat;
			in++;
			return;
		}
		else if(in==4)				//��ȡ��λLED����
		{
			l=dat;
			in++;
			return;
		}
		else if(in==5)				//��ȡУ��ֵ
		{
			o=dat;
			if(o!=(h^l^0x55)) in=0;		//У��
			else in++;
			return;
		}
		else if(in>=6)				//�������ݶ�ȡ�׶�
		{
			rgb[(in-6)/3][(in-6)%3]=dat;//������д������
			if((in-6)/3==l)				//���ݷ�����led����ȷ�������Ƿ�������
			{
				in=0;
				EA=0;
				return;
			}
			else in++;
			return;
		}
			else  			//���inС��6������;�����жϡ������¿�ʼ��
			{
				in=0;
				return;
			}
	}
}