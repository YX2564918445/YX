# include "reg52.h"
// 引脚定义
sbit LED1 = P2^0;  		// LED1定义
sbit BEEP = P2^5;		// 蜂鸣器定义
sbit KEY1 = P3^1;		// 按键1定义
sbit KEY2 = P3^0;		// 按键2定义
sbit KEY3 = P3^2;		// 按键3定义
sbit KEY4 = P3^3;		// 按键4定义
# define ALL_LED  P2 	// 所有LED控制
// 变量定义
unsigned char i  ;      // 循环变量定义
unsigned char j  ;
unsigned char mode1 = 0;// 按键1触发状态
unsigned char mode2 = 0;// 按键2触发状态

// 毫秒级延迟函数
void delay_ms(unsigned int time) 
{
	unsigned int j , i;

	for( i = 0; i < time; i++)

		for( j = 0; j < 144 ; j++);
}

// 按键1功能（LED1亮灭十次）
void key1_function()
{
	for(i = 1;i <= 20;i++)
	{
		LED1 = ~LED1;
		delay_ms(500);
	}
	mode1 = 0;			// 按键1状态重置
}

// 按键2功能（所有LED亮灭十次）
void key2_function()
{
	for(i = 1;i <= 10;i++)
	{
		ALL_LED = 0X00;	// 所有LED亮
		delay_ms(500);
		ALL_LED = 0XFF; // 所有LED灭
		delay_ms(500);
	}
	mode2 = 0;			// 按键2状态重置
}

// LED向右循环点亮
void led_right()
{
	ALL_LED = ~0X01;
	for(i = 0 ; i < 8 ; i++ ) 
	{
		ALL_LED = ~(0X01 << i);
		delay_ms(500);
	}
}

//蜂鸣器响5次
void beep()
{
	for(i = 0;i < 5;i++)
	{
		for(j = 0; j < 200;j++)
		{
			BEEP = ~BEEP;
			delay_ms(1);
		}
		BEEP = 1;
		delay_ms(300);
	}
}

//外部中断0服务函数
void exti0() interrupt 0 
{
	delay_ms(20);				// 消抖
	if(KEY3==0)					// 再次判断K3键是否按下
		led_right();			// 执行外部中断0功能
	ALL_LED = 0XFF;				// 关闭LED
}

//外部中断1服务函数
void exti1() interrupt 2 
{
	delay_ms(20);				// 消抖
	if(KEY4==0)					// 再次判断K4键是否按下
		beep();					// 执行外部中断1功能			
}
// 主函数
void main()
{
	ALL_LED = 0XFF ; 					// 初始化所有LED的状态
	// 外部中断配置
	IT0 = 1 ;                           // INT0下降沿触发
	IT1 = 1 ;							// INT1下降沿触发
	EX0 = 1 ;    						// 使能INT0
	EX1 = 1 ;							// 使能INT1
	PX1 = 1 ;							// 设置INT1为最高优先级
	EA  = 1 ;							// 全局中断使能
	while(1)
	{
		if(KEY1 == 0)					// 按键1检测
		{
			delay_ms(20);				// 消抖
			if(KEY1 == 0)
			{
				mode1 = 1;				// 按键1状态改变
				while(KEY1 == 0);		// 等待按键释放
			}
		}
		if(KEY2 == 0)					// 按键2检测
		{
			delay_ms(20);				// 消抖
			if(KEY2 == 0)
			{
				mode2 = 1;				// 按键2状态改变
				while(KEY2 == 0);		// 等待按键释放
			}
		}
		if(mode1)						// 执行按键1功能
			key1_function();
		if(mode2)						// 执行按键2功能
			key2_function();
	}
}