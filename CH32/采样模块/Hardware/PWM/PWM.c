#include "debug.h"
#include "PWM.h"

// 初始化TIM3_CH1（PA6）为PWM输出模式
// arr: 自动重装载值（决定PWM周期）
// psc: 预分频系数（决定定时器时钟频率）
// ccr: 比较值（决定PWM占空比）
// 系统时钟为72MHz
void TIM3_PWM_Init(uint16_t arr, uint16_t psc, uint16_t ccr)
{
    // 使能TIM3时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    // GPIO初始化
    GPIO_InitTypeDef GPIO_InitStructure;
    // 使能GPIOA和复用功能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE); 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = PWM1_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(PWM1_PORT,&GPIO_InitStructure);
    // 初始化定时器
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = arr; // 自动重装载值（周期 = (arr+1) * (psc+1) / 定时器时钟频率）
    TIM_TimeBaseStructure.TIM_Prescaler = psc;// 预分频系数
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 时钟分频（不分频）
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    // PWM初始化
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; // PWM模式1：CNT < CCR时输出有效电平
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 使能输出
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; // 有效电平为高电平
    TIM_OCInitStructure.TIM_Pulse = ccr; // 比较值（占空比 = ccr / (arr+1) * 100%）
    TIM_OC1Init(TIM3, &TIM_OCInitStructure); // 初始化CH1
    // 使能CH1预装载（保证CCR值更新时平滑过渡）
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
    // 使能定时器自动重装载预装载
    TIM_ARRPreloadConfig(TIM3, ENABLE);
    // 启动定时器
    TIM_Cmd(TIM3, ENABLE);
}

// 初始化TIM4_CH1（PB6）为PWM输出模式
// arr: 自动重装载值（决定PWM周期）
// psc: 预分频系数（决定定时器时钟频率）
// ccr: 比较值（决定PWM占空比）
// 系统时钟为72MHz
void TIM4_PWM_Init(uint16_t arr, uint16_t psc, uint16_t ccr)
{
    // 使能TIM4时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    // GPIO初始化
    GPIO_InitTypeDef GPIO_InitStructure;
    // 使能GPIOB和复用功能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE); 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = PWM2_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(PWM2_PORT,&GPIO_InitStructure);
    // 初始化定时器
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = arr; // 自动重装载值（周期 = (arr+1) * (psc+1) / 定时器时钟频率）
    TIM_TimeBaseStructure.TIM_Prescaler = psc;// 预分频系数
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 时钟分频（不分频）
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    // PWM初始化
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; // PWM模式1：CNT < CCR时输出有效电平
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 使能输出
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; // 有效电平为高电平
    TIM_OCInitStructure.TIM_Pulse = ccr; // 比较值（占空比 = ccr / (arr+1) * 100%）
    TIM_OC1Init(TIM4, &TIM_OCInitStructure); // 初始化CH1
    // 使能CH1预装载（保证CCR值更新时平滑过渡）
    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
    // 使能定时器自动重装载预装载
    TIM_ARRPreloadConfig(TIM4, ENABLE);
    // 启动定时器
    TIM_Cmd(TIM4, ENABLE);
}
