#include "debug.h"
#include "ch32v20x_adc.h"
#include "ADC.h"

/*ADC初始化*/
void AD_Init(void)
{
    // 开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE); // ADC1时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);// GPIOA时钟
    // ADC分频数
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);
    // 初始化GPIO
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    // ADC填充菜单列表
    ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_13Cycles5);
    ADC_RegularChannelConfig(ADC1,ADC_Channel_1,2,ADC_SampleTime_13Cycles5);
    // ADC初始化
    ADC_InitTypeDef ADC_InitStructure;
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1,&ADC_InitStructure);
    // 开启ADC电源
    ADC_Cmd(ADC1,ENABLE);
    // ADC校准
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1) == SET);
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1) == SET);
}

/*获取ADC采样值*/
uint16_t AD_GetValue_0(void)
{
    ADC_ClearFlag(ADC1, ADC_FLAG_EOC); // 清除EOC标志
    ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_13Cycles5);
    ADC_SoftwareStartConvCmd(ADC1,ENABLE);
    while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC) == RESET);
    return ADC_GetConversionValue(ADC1);
}

uint16_t AD_GetValue_1(void)
{
    ADC_ClearFlag(ADC1, ADC_FLAG_EOC); // 清除EOC标志
    ADC_RegularChannelConfig(ADC1,ADC_Channel_1,1,ADC_SampleTime_13Cycles5);
    ADC_SoftwareStartConvCmd(ADC1,ENABLE);
    while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC) == RESET);
    return ADC_GetConversionValue(ADC1);
}

