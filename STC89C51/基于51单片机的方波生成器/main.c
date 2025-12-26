#include <reg52.h>

sbit PWM_OUT = P2^0; // PWM输出引脚

unsigned int Timer0_Reload; // 定时器0重载值 (控制高电平时间)
unsigned int Timer1_Reload; // 定时器1重载值 (控制低电平时间)

// 定时器0初始化
void Timer0_Init(void)
{
    TMOD &= 0xF0; // 清空定时器0模式位
    TMOD |= 0x01; // 定时器0工作在模式1
    // 定时器0中断使能
    ET0 = 1;
    // 总中断使能
    EA = 1;
}

// 定时器1初始化
void Timer1_Init(void)
{
    TMOD &= 0x0F; // 清空定时器1模式位
    TMOD |= 0x10; // 定时器1工作在模式1 (16位自动重装)
    // 定时器1中断使能
    ET1 = 1;
    // 总中断使能
    EA = 1;
}

/**
 * @brief 
 * @param period 周期
 * @param duty_cycle 占空比
 */
void Set_PWM_Params(unsigned int period, unsigned char duty_cycle)
{
    unsigned long temp;
	// 防止错误参数
    if (period == 0) period = 1;
    if (duty_cycle > 100) duty_cycle = 100;
    if (duty_cycle == 0) 
    {
        PWM_OUT = 0;
        return;
    }
    if (duty_cycle == 100) 
    {
        PWM_OUT = 1;
        return;
    }
    
    // 计算高电平时间和低电平时间
    // 高电平计数值 = (周期 * 占空比) / 100
    temp = (unsigned long)period * duty_cycle;
    Timer0_Reload = 65536 - (temp / 100); // 定时器0控制高电平
    // 低电平计数值 = 周期 - 高电平计数值
    temp = (unsigned long)period * (100 - duty_cycle);
    Timer1_Reload = 65536 - (temp / 100); // 定时器1控制低电平
    // 启动第一个周期
    PWM_OUT = 1; // 先输出高电平
    // 装载并启动定时器0
    TH0 = (unsigned char)(Timer0_Reload >> 8);
    TL0 = (unsigned char)Timer0_Reload;
    TR0 = 1; // 启动定时器0
}

// 定时器0中断服务函数（高->低）
void Timer0_ISR(void) interrupt 1
{
    PWM_OUT = 0; // 切换到低电平
    TR0 = 0; // 关闭定时器0
    // 装载并启动定时器1 (低电平时间)
    TH1 = (unsigned char)(Timer1_Reload >> 8);
    TL1 = (unsigned char)Timer1_Reload;
    TR1 = 1; // 启动定时器1
}

// 定时器1中断服务函数（低->高）
void Timer1_ISR(void) interrupt 3
{
    PWM_OUT = 1; // 切换到高电平
    TR1 = 0; // 关闭定时器1    
    // 装载并启动定时器0 (高电平时间)
    TH0 = (unsigned char)(Timer0_Reload >> 8);
    TL0 = (unsigned char)Timer0_Reload;
    TR0 = 1; // 启动定时器0
}

// 主函数
void main(void)
{
    unsigned int pwm_period = 2000; // PWM周期（1->1us）
    unsigned char pwm_duty = 80;    // PWM占空比
    Timer0_Init();
    Timer1_Init();
    // 设置并启动PWM
    Set_PWM_Params(pwm_period, pwm_duty); 
    while(1)
    {
        
    }
}