#include "debug.h"
#include "MQ_2.h"

// MQ_2初始化
void MQ_2_Init(void)
{
    // 开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1,ENABLE);
    // GPIO初始化
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Pin = MQ2_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MQ2_PORT,&GPIO_InitStructure);
    // ADC初始化
    ADC_InitTypeDef ADC_InitStructure;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_Init(ADC1,&ADC_InitStructure);
    // 配置ADC通道
    ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_55Cycles5);
    // 使能ADC
    ADC_Cmd(ADC1,ENABLE);
    // ADC 校准
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));
}

// 读取 ADC 原始值
uint16_t MQ2_ReadRaw(void) 
{
    // 启动转换
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    // 等待转换结束
    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    // 返回结果
    return ADC_GetConversionValue(ADC1);
}

// 读取电压值 (Vref = 3.33V)
float MQ2_ReadVoltage(void) 
{
    uint16_t raw = MQ2_ReadRaw();
    return (float)raw / 4095.0f * 3.33f;
}

// 气体检测和换算
uint16_t MQ2_GasGet(void) 
{
    float voltage = MQ2_ReadVoltage();
    uint16_t Gas;
    Gas = (uint16_t)(voltage * 1000);
    return Gas; 
    
}
