# include "reg52.h"
// 引脚定义
sbit KEY1 = P3^1;  // 按键1定义
sbit KEY2 = P3^0;  // 按键2定义
sbit KEY3 = P3^2;  // 按键3定义
sbit KEY4 = P3^3;  // 按键4定义
sbit BEEP = P2^5;  // 蜂鸣器引脚定义
// 变量定义
unsigned char KEY_state = 0 ;										// 按键34状态
unsigned int current_freq = 2001 ;									// 当前频率对应周期
unsigned int max_current_freq = 1 ;									// 蜂鸣器最大极限频率对应周期
unsigned int min_current_freq = 2001 ;								// 蜂鸣器最小极限频率对应周期
unsigned int scale_freq[21] = {382, 340, 303, 286, 255, 227, 203,  
							  191, 170, 152, 143, 128, 114, 101, 
							  95,  85,  76,  72,  64,  57,  51};    // 音阶频率对应周期
unsigned int scale_index = 1 ;										// 当前音阶索引
unsigned int i,n,m = 1;                                             // 循环参数定义

// 毫秒级延迟函数
void delay_ms(unsigned int time) 
{
	unsigned int j , i;

	for( i = 0; i < time; i++)

		for( j = 0; j < 144 ; j++);
}

// 10微秒延迟函数
void delay_10us(unsigned int ten_us)
{
	while(ten_us--);	
}

// 蜂鸣器连续变化频率函数
void beep_continuity()
{
	unsigned int a = 2;									// 防止高频时过快
	if(m >= 60)
		a = m/10;
	else 
		a = 2;
	for(n = 1;n <= a ;n++)
	{	
		BEEP = ~BEEP;									// 引脚电平取反
		delay_10us( current_freq);						// 按当前周期延时
	}
}

// 按键12扫描处理函数
void key_scan()
{
	// 按键1处理
	if ( KEY1 == 0 )								// 按键1按下
	{	
		delay_ms(10);								// 消抖
		while( KEY1 == 0 )
		{
			beep_continuity();						// 调用蜂鸣器连续变化频率函数
			if (current_freq <= max_current_freq)	// 防止超过最小周期
				current_freq = max_current_freq;
			else 
			{
				current_freq -= 10;					// 周期改变
				m ++;
			}
		}
	}
	// 按键2处理
	if ( KEY2 == 0 )								// 按键2按下
	{	
		delay_ms(10);								// 消抖
		while( KEY2 == 0 )
		{
			beep_continuity();						// 调用蜂鸣器连续变化频率函数
			if (current_freq > min_current_freq)	// 防止超过最大周期
				current_freq = min_current_freq;	
			else 
			{
				current_freq += 10;					// 周期改变
				m--;
			}
		}	
	}
}

// 蜂鸣器音阶变化函数(和按键34处理)
void beep_scale_freq()
{
	if (KEY3 == 0)					// 按键3按下
	{
		delay_ms(20);				// 消抖
		while (KEY3 == 0) ;			// 等待按键松手
		KEY_state = 1;				// 按键3状态改变
		scale_index += 1 ;			// 音阶序号增加
		if (scale_index > 21)		// 防止超出范围
			scale_index = 21 ;
	}
	if (KEY4 == 0)      			// 按键4按下
	{
		delay_ms(20);				// 消抖
		while(KEY4 == 0);			// 等待按键松手
		KEY_state = 1;				// 按键4状态改变
		scale_index -= 1 ;			// 音阶序号减少
		if (scale_index < 1)		
			scale_index = 1 ;		// 防止超出范围
	}
	if ( KEY_state == 1)
	{
		for(i = scale_index * 25 ; i > 0 ; i--)	// 蜂鸣器在检测到有按键按下时鸣响对应音阶一段时间
		{
			BEEP = ~BEEP;
			delay_10us(scale_freq[scale_index - 1]);
		}
		KEY_state = 0;				// 按键34状态重置
	}
}

// 主函数
void main()
{
	while(1)
	{
		key_scan();					// 调用按键12扫描处理函数
		beep_scale_freq();			// 调用蜂鸣器音阶变化函数
	}
}