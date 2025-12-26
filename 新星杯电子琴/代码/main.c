#include "reg52.h"

#define KEY_LINE P2    // 行
#define KEY_COLUMN P0  // 列


sbit BEEP = P3^3;	  // 蜂鸣器
sbit  LED = P3^4;	  // LED

unsigned char scale_freq[21] = {96,  85,  76,  72,  64,  57,  50 ,  
							  191, 170, 152, 143, 128, 114, 101 ,
							  382, 340, 303, 286, 255, 227, 202};   
						// 音阶频率对应周期

// 毫秒级延迟函数
void delay(unsigned int time) 
{
	unsigned int j , i;
	for( i = 0; i < time; i++)
		for( j = 0; j < 144 ; j++);
}

// 10us延迟函数
void delay_10us(unsigned int ten_us)
{
	while(ten_us--);	
}

// 蜂鸣器函数
void beep()
{
	BEEP = ~BEEP;
}

void main()
{
	while(1)
	{
		// 第一列
		KEY_LINE = 0XFF;
		KEY_COLUMN = 0XBF;
		if(KEY_LINE != 0XFF)
		{
			delay(20);
			if(KEY_LINE != 0XFF)
			{
				LED = 0;
				while(KEY_LINE == 0XFB) // do' 1
				{beep();delay_10us(scale_freq[0]);}
				while(KEY_LINE == 0XFD) // do  8
				{beep();delay_10us(scale_freq[7]);}
				while(KEY_LINE == 0XFE) // do, 15
			{	beep();delay_10us(scale_freq[14]);}
			}
		}
		// 第二列
		KEY_LINE = 0XFF;
		KEY_COLUMN = 0XDF;
		if(KEY_LINE != 0XFF)
		{
			delay(20);
			if(KEY_LINE != 0XFF)
			{
				LED = 0;
				while(KEY_LINE == 0XFB) // re' 2
				{beep();delay_10us(scale_freq[1]);}
				while(KEY_LINE == 0XFD) // re  9
				{beep();delay_10us(scale_freq[8]);}
				while(KEY_LINE == 0XFE) // re, 16
				{beep();delay_10us(scale_freq[15]);}
			}
		}
		// 第三列
		KEY_LINE = 0XFF;
		KEY_COLUMN = 0XEF;
		if(KEY_LINE != 0XFF)
		{
			delay(20);
			if(KEY_LINE != 0XFF)
			{
				LED = 0;
				while(KEY_LINE == 0XFB) // mi' 3
				{beep();delay_10us(scale_freq[2]);}
				while(KEY_LINE == 0XFD) // mi  10
				{beep();delay_10us(scale_freq[9]);}
				while(KEY_LINE == 0XFE) // mi, 17
				{beep();delay_10us(scale_freq[16]);}
			}
		}
		// 第四列
		KEY_LINE = 0XFF;
		KEY_COLUMN = 0XF7;
		if(KEY_LINE != 0XFF)
		{
			delay(20);
			if(KEY_LINE != 0XFF)
			{
				LED = 0;
				while(KEY_LINE == 0XFB) // fa' 4
				{beep();delay_10us(scale_freq[3]);}
				while(KEY_LINE == 0XFD) // fa  11
				{beep();delay_10us(scale_freq[10]);}
				while(KEY_LINE == 0XFE) // fa, 18
				{beep();delay_10us(scale_freq[17]);}
			}
		}
		// 第五列
		KEY_LINE = 0XFF;
		KEY_COLUMN = 0XFB;
		if(KEY_LINE != 0XFF)
		{
			delay(20);
			if(KEY_LINE != 0XFF)
			{
				LED = 0;
				while(KEY_LINE == 0XFB) // sol' 5
				{beep();delay_10us(scale_freq[4]);}
				while(KEY_LINE == 0XFD) // sol  12
				{beep();delay_10us(scale_freq[11]);}
				while(KEY_LINE == 0XFE) // sol, 19
				{beep();delay_10us(scale_freq[18]);}
			}
		}
		// 第六列
		KEY_LINE = 0XFF;
		KEY_COLUMN = 0XFD;
		if(KEY_LINE != 0XFF)
		{
			delay(20);
			if(KEY_LINE != 0XFF)
			{
				LED = 0;
				while(KEY_LINE == 0XFB) // la' 6
				{beep();delay_10us(scale_freq[5]);}
				while(KEY_LINE == 0XFD) // la  13
				{beep();delay_10us(scale_freq[12]);}
				while(KEY_LINE == 0XFE) // la, 20
				{beep();delay_10us(scale_freq[19]);}
			}
		}
		// 第七列
		KEY_LINE = 0XFF;
		KEY_COLUMN = 0XFE;
		if(KEY_LINE != 0XFF)
		{
			delay(20);
			{
				LED = 0;
				while(KEY_LINE == 0XFB) // si' 7
				{beep();delay_10us(scale_freq[6]);}
				while(KEY_LINE == 0XFD) // si  14
				{beep();delay_10us(scale_freq[13]);}
				while(KEY_LINE == 0XFE) // si, 21
				{beep();delay_10us(scale_freq[20]);}
			}
		}
		LED = 1;
	}
}