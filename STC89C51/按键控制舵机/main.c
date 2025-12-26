# include "reg52.h"
# define uchar unsigned char
# define uint unsigned int
// 引脚定义
sbit KEY1 = P3^1; 	// 左转
sbit KEY2 = P3^0;	// 停止
sbit KEY3 = P3^2;	// 右转
sbit Servo = P1^0;	// 舵机信号线
// 变量声明
uint high_time = 6;	// 7.5%占空比，高电平6*0.25ms
uint i = 0;			// 循环变量
uint j = 0;

// 定时器0初始化函数
void Timer0Init()	
{
	TMOD |= 0X01;  	// 设置工作模式为16位定时器
	TH0 = 0XFF;		// 定时0.25ms
	TL0 = 0X06;
	ET0 = 1;
	EA = 1;
	TR0 = 1; 
}

// 毫秒级延迟函数
void delay_ms(uint time) 
{
	uint j , i;

	for( i = 0; i < time; i++)

		for( j = 0; j < 144 ; j++);
}

// 按键扫描函数
void key_scanf()
{
	if(KEY1 == 0||KEY2 == 0||KEY3 == 0)
	{
		delay_ms(10);  				// 消抖 
		if(KEY1 == 0)
		{
			high_time = 2;
		}
		while(KEY1 == 0);
		if(KEY2 == 0)
			high_time = 6;
		while(KEY2 == 0);
		if(KEY3 == 0)
		{
			high_time = 10;
		}
		while(KEY3 == 0);
	}
}
// 主函数
void main()
{
	Timer0Init();					// 初始化定时器
	while(1)
	{
		key_scanf();
	}
}

// 定时器中断服务函数
void Timer0Handler() interrupt 1
{
	TH0 = 0XFF;		// 定时0.25ms
	TL0 = 0X06;
	i++;
	if(i < high_time)
		Servo = 1;
	else if(i < 80)
		Servo = 0;
	else
	{
		Servo = 1;
		i = 0;    	// 循环变量重置
	}
}