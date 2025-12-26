#include <reg52.h>
#include <intrins.h>
// 引脚定义
sbit Trig = P1^0;   // 超声波Trig引脚 -> P1.0
sbit Echo = P1^1;   // 超声波Echo引脚 -> P1.1
sbit Buzzer = P2^5; // 无源蜂鸣器 -> P2.5
// 全局变量
unsigned int distance = 0;      // 存储测量距离（厘米）
unsigned int echo_time = 0;     // 存储Echo高电平时间
bit alarm_enabled = 0;          // 报警使能标志
//定时器0初始化（用于蜂鸣器）
void Timer0_Init() 
{
    TMOD &= 0xF0;   // 清除定时器0设置
    TMOD |= 0x01;   // 定时器0模式1（16位定时）
    TH0 = (65536 - 250) / 256; // 定时250us
    TL0 = (65536 - 250) % 256;
    ET0 = 1;        // 允许定时器0中断
    TR0 = 1;        // 启动定时器0
    EA = 1;         // 开启总中断
}
// 定时器1初始化（用于超声波测距计时）
void Timer1_Init() 
{
    TMOD &= 0x0F;   // 清除定时器1设置
    TMOD |= 0x10;   // 定时器1模式1（16位定时）
    TH1 = 0;
    TL1 = 0;
    ET1 = 0;        // 禁用定时器1中断（只用作计数器）
}
// 微秒级延迟
void Delay_us(unsigned int t) 
{
    while (t--) 
	{
        unsigned char i = 2;  // 根据11.0592MHz调整
        while (i--);
    }
}
// 毫秒级延迟
void Delay_ms(unsigned int t) 
{
    unsigned int i, j;
    for (i = 0; i < t; i++)
        for (j = 0; j < 110; j++);
}
// 超声波测距函数
void Measure_Distance() {
    unsigned int timeout = 0; 
    // 发送触发信号
    Trig = 0;
    Delay_us(2);
    Trig = 1;
    Delay_us(12);   // 持续12us高电平
    Trig = 0;
    // 等待Echo回波高电平（加入超时保护）
    while (!Echo && timeout < 5000) { 
        timeout++;
        Delay_us(1);
    }
    if (timeout >= 5000) {
        distance = 999; // 超时设置大距离值
        return;
    }
    // 启动定时器1计时
    TR1 = 1;
    TH1 = 0;
    TL1 = 0;
    // 等待Echo回波结束（加入超时保护）
    timeout = 0;
    while (Echo && timeout < 25000) { // 最大测量距离约42cm
        timeout++;
        Delay_us(1);
    }
    TR1 = 0;        // 停止计时
    // 计算高电平时间（单位：微秒）
    echo_time = (TH1 << 8) | TL1;
    // 计算距离（单位：厘米）: 时间*声速(340m/s)/2
    distance = (unsigned int)(echo_time * 0.017); 
}
// 主函数
void main() 
{
    // 初始化所有引脚
    Trig = 0;
    Echo = 0;
    Buzzer = 0;  // 蜂鸣器初始关闭
    Timer0_Init();  // 初始化定时器0（蜂鸣器）
    Timer1_Init();  // 初始化定时器1（超声波）
    while (1) 
	{
        Measure_Distance();  // 测量距离
        // 距离判断与蜂鸣器控制
        if (distance < 5) 
            alarm_enabled = 1;  // 使能报警
		else 
            alarm_enabled = 0;  // 关闭报警
        Delay_ms(100); // 延时100ms后重新测量
    }
}
// 定时器0中断服务函数（蜂鸣器PWM生成）
void Timer0_ISR() interrupt 1 
{
    static bit buzzer_state = 0;
    // 重新装载定时器初值（250us）
    TH0 = (65536 - 250) / 256;
    TL0 = (65536 - 250) % 256;
    // 如果报警使能，生成2kHz方波
    if (alarm_enabled) 
	{
        buzzer_state = !buzzer_state;  // 翻转状态
        Buzzer = buzzer_state;         // 输出到蜂鸣器
    } else 
        Buzzer = 0;  // 关闭蜂鸣器
}