/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/06/06
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 *USART Print debugging routine:
 *USART1_Tx(PA9).
 *This example demonstrates using USART1(PA9) as a print debug port output.
 *
 */
#include "debug.h"
#include "OLED.h"
#include "PWM.h"
#include "ADC.h"
#include "ModBus.h"
/* Global typedef */

/* Global define */
// 采样基准电压
#define reference_voltage_V 1.64
#define reference_voltage_I 1.53
/* Global Variable */
// 两路PWM输出占空比
uint16_t pwm1 = 500,pwm2 =800;
// 电压电流采样相关数据
float voltage = 0.0 , electricity = 0.0;        // 当前电流电压真实值
float adc_voltage = 0.0 , adc_electricity = 0.0;// 当前ADC采样通道电压
uint16_t voltage_IN = 0 , electricity_IN = 0;   // 采样通道ADC原始值
// 循环变量定义
uint16_t i,j;

/*函数定义*/
// 系统初始化
void System_Init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    ModBus_Init();
    // ADC初始化
    AD_Init();
    // 模块初始化
    OLED_Init();
    OLED_Clear();
    // 显示初始化
    OLED_ShowString(1,1, "V:   .    V");
    OLED_ShowString(2,1, "I:  .    A");
    OLED_ShowString(3,1,"PWM1:    %");
    OLED_ShowString(4,1,"PWM2:    %");
    // 初始化PWM
    TIM3_PWM_Init(999,71,0);    // PWM1
    TIM4_PWM_Init(999,71,0);    // PWM2
    TIM_SetCompare1(TIM3, pwm1);// 设置PWM1占空比
    TIM_SetCompare1(TIM4, pwm2);// 设置PWM2占空比  
}

// ±36V电压采样计算函数
void Voltage_Get(void)
{
    uint16_t voltage_IN_1,voltage_IN_2,voltage_IN_3;
    // 多次采样取平均消除误差
    voltage_IN_1 =  AD_GetValue_0();
    Delay_Ms(20);
    voltage_IN_2 =  AD_GetValue_0();
    Delay_Ms(20);
    voltage_IN_3 =  AD_GetValue_0();
    // 获取ADC原始值
    voltage_IN = (voltage_IN_1+voltage_IN_2+voltage_IN_3)/3;           
    adc_voltage = (voltage_IN/4095.0)*3.3f; // 计算ADC通道电压值
    // 区分正负
    if (adc_voltage > reference_voltage_V + 0.05)
    {
        OLED_ShowString(1,3,"+");
        voltage = ((adc_voltage - reference_voltage_V)/0.55*12)-0.2;
    }
    else if (adc_voltage < reference_voltage_V - 0.05)
    {
        OLED_ShowString(1,3,"-");
        voltage = ((reference_voltage_V -adc_voltage)/0.55*12)+0.2;
    }
    else
    {
        OLED_ShowString(1,3," ");
        voltage = 0;
    }
    
}

// ±5A电流采样计算函数
void Electricity_Get(void)
{
    uint16_t electricity_IN_1,electricity_IN_2,electricity_IN_3;
    // 多次采样取平均消除误差
    electricity_IN_1 = AD_GetValue_1();
    Delay_Ms(10);
    electricity_IN_2 = AD_GetValue_1();
    Delay_Ms(10);
    electricity_IN_3 = AD_GetValue_1();
    // 获取ADC原始值
    electricity_IN = (electricity_IN_1 + electricity_IN_2 + electricity_IN_3)/3;        
    adc_electricity = (electricity_IN/4095.0)*3.3f; // 计算ADC通道电压值
    // 区分正负
    if (adc_electricity > reference_voltage_I + 0.1)
    {
        OLED_ShowString(2,3,"+");
        electricity = (adc_electricity - reference_voltage_I)*4.1096;
    }
    else if(adc_electricity < reference_voltage_I - 0.1)
    {
        OLED_ShowString(2,3,"-");
        electricity = (reference_voltage_I - adc_electricity)*4.1096;
    }
    else
    {
        OLED_ShowString(2,3," ");
        electricity = 0;
    }
}

// OLED显示更新
void OLED_Updata(void)
{
    // 显示当前电压
    OLED_ShowNum(1,4,(uint16_t)voltage,2);
    OLED_ShowNum(1,7,(uint16_t)((voltage - (uint16_t)voltage)*1000),3);
    // 显示当前电流
    OLED_ShowNum(2,4,(uint16_t)electricity,1);
    OLED_ShowNum(2,6,(uint16_t)((electricity - (uint16_t)electricity)*1000),3);
    // OLED显示当前输出PWM的占空比
    OLED_ShowNum(3,6,pwm1/10,3);
    OLED_ShowNum(4,6,pwm2/10,3);
}

/*主函数*/
int main(void)
{
    // 系统初始化
    System_Init();   
    while(1)
    {
        // 获取当前的电压电流值
        Voltage_Get();
        Delay_Ms(200);
        Electricity_Get();
        // 显示更新
        OLED_Updata();
        // 采样间隔
        Delay_Ms(200);
    }
}
