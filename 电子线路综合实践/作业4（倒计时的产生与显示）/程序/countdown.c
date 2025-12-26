# include "reg52.h"

// 引脚定义
# define SMG_D P0					// 数码管段选
sbit SMG_A = P2^2;					// 数码管位选控制引脚定义
sbit SMG_B = P2^3;
sbit SMG_C = P2^4;
sbit BEEP = P2^5;					// 蜂鸣器定义

// 变量定义
unsigned char smg_code[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
				0x7f,0x6f};			// 共阴极数码管显示0~9的段码数据
unsigned int now_count = 30;		// 当前倒计时剩余
unsigned int i = 0;				    // 计时器循环变量

// 毫秒级延迟函数
void delay_ms(unsigned int time) 
{
	unsigned int j , i;

	for( i = 0; i < time; i++)

		for( j = 0; j < 144 ; j++);
}

// 蜂鸣器函数
void beep()
{
	unsigned int i;
	for(i = 0;i < 500;i++)
	{
		BEEP = ~BEEP;
		delay_ms(1);
	}
}

// 串口发送当前倒计时
void send_count(count)  
{
	SBUF = count;
	while(!TI);		//等待发送数据完成
	TI = 0;			//清除发送完成标志位	
}

//定时器0中断函数
void time0() interrupt 1 
{
	TH0=0XFC;									// 给定时器赋初值，定时1ms
	TL0=0X18;
	i++;
	if(i == 1000)
	{
		i = 0;
		now_count--;							// 当前倒计时更新
		send_count(now_count);					// 发送当前倒计时	
		if(now_count == 0)						// 倒计时结束
			 TR0 = 0;							// 关闭定时器
	}
}

// 倒计时显示函数
void display_count()
{
	SMG_A = 0,SMG_B = 0,SMG_C = 0;						// 显示个位
	SMG_D = smg_code[now_count % 10];
	delay_ms(1);
	SMG_D = 0X00;										// 消隐
	if((now_count -(now_count % 10))/10 != 0)
	{
		SMG_A = 1,SMG_B = 0,SMG_C = 0;					// 显示十位
		SMG_D = smg_code[(now_count -(now_count % 10))/10];
		delay_ms(1);
		SMG_D = 0X00;									// 消隐
	}
}
// 主函数
void main()
{
	TMOD|=0X01;					// 选择为定时器0模式，工作方式1
	TH0=0XFC;					// 给定时器赋初值，定时1ms
	TL0=0X18;	
	ET0=1;						// 打开定时器0中断允许
	
	TMOD|=0X21;					// 设置T1工作方式2
	SCON=0X50;					// 串口模式
	TH1=0XFD;					// 波特率9600
	TL1=0XFD;
	TR1 =1;     				// 启动T1
	
	EA=1;						// 打开总中断
	TR0 = 1 ;					// 开启定时器
	ES = 1;						// 允许串口中断
	while(1)
	{
		if(now_count == 0)										// 倒计时结束时
		{
			SMG_A = 0,SMG_B = 0,SMG_C = 0;						// 显示0
			SMG_D = smg_code[0];
			beep();												// 蜂鸣器间歇提醒
			delay_ms(500);
		}
		else
			display_count();									// 数码管显示当前倒计时
	}
}