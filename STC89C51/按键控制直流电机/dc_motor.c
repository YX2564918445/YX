# include "reg52.h"
// 引脚定义
sbit DC_Motor = P1^0;
sbit KEY1 = P3^1;
// 变量定义
unsigned char motor_mode = 0;

// 毫秒级延迟
void delay_ms(unsigned int t) 
{
    unsigned int i, j;
    for (i = 0; i < t; i++)
        for (j = 0; j < 110; j++);
}

// 按键扫描函数
void key_scanf()
{
	if(KEY1 == 0)
	{
		delay_ms(20); 		// 消抖 
		if(KEY1 == 0)
			motor_mode = ~motor_mode;
		while(KEY1 == 0);	// 等待按键释放
	}
}

// 主程序
void main()
{
	DC_Motor = 0;
	while(1)
	{
		key_scanf();
		if(motor_mode)
			DC_Motor = 1;
		else
			DC_Motor = 0;
	}
}