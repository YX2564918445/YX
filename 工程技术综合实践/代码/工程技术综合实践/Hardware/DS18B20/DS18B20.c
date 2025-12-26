#include "debug.h"
#include "DS18B20.h"

// DS18B20设置输出模式
void DS18B20_Set_Output(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = DS18B20_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DS18B20_PORT, &GPIO_InitStructure);
}

// DS18B20设置为输入模式
void DS18B20_Set_Input(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = DS18B20_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  // 上拉输入
    GPIO_Init(DS18B20_PORT, &GPIO_InitStructure);
}

// 复位DS18B20
void DS18B20_Reset(void)
{
    DS18B20_Set_Output();
    GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);  // 拉低总线
    Delay_Us(750);                              // 延时750us
    GPIO_SetBits(DS18B20_PORT, DS18B20_PIN);    // 释放总线
    Delay_Us(20);                               // 延时20us
}

// 检测DS18B20存在
uint8_t DS18B20_Check(void)
{
    u8 time_temp = 0;
    DS18B20_Set_Input();
    // 等待总线拉低
    while(GPIO_ReadInputDataBit(DS18B20_PORT, DS18B20_PIN) && time_temp < 20)
    {
        time_temp++;
        Delay_Us(10);
    }
    if(time_temp >= 20) 
        return 1;  // 未检测到设备
    else 
        time_temp = 0;
    // 等待总线释放
    while(!GPIO_ReadInputDataBit(DS18B20_PORT, DS18B20_PIN) && time_temp < 20)
    {
        time_temp++;
        Delay_Us(10);
    }
    if(time_temp >= 20) 
        return 1;  // 未检测到设备
    return 0;                      // 检测到设备
}

// DS18B20读取一个位
uint8_t DS18B20_Read_Bit(void)
{
    uint8_t dat = 0;
    DS18B20_Set_Output();
    GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);  // 拉低总线
    __NOP();__NOP();                            // 短暂延时
    GPIO_SetBits(DS18B20_PORT, DS18B20_PIN);    // 释放总线
    DS18B20_Set_Input();
    __NOP();__NOP();                            // 等待数据稳定
    if(GPIO_ReadInputDataBit(DS18B20_PORT, DS18B20_PIN))
        dat = 1;
    else
        dat = 0;
    Delay_Us(50);  // 满足时序要求
    return dat;
}

// DS18B20读取一个字节
uint8_t DS18B20_Read_Byte(void)
{
    uint8_t i = 0;
    uint8_t dat = 0;
    uint8_t temp = 0;
    for(i = 0; i < 8; i++)
    {
        temp = DS18B20_Read_Bit();
        dat = (temp << 7) | (dat >> 1);
    }
    return dat;
}

// 写入一个字节
void DS18B20_Write_Byte(uint8_t dat)
{
    uint8_t i = 0;
    uint8_t temp = 0;
    DS18B20_Set_Output();
    for(i = 0; i < 8; i++)
    {
        temp = dat & 0x01;
        dat >>= 1;
        if(temp)
        {
            GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
            __NOP();__NOP();
            GPIO_SetBits(DS18B20_PORT, DS18B20_PIN);
            Delay_Us(60);
        }
        else
        {
            GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
            Delay_Us(60);
            GPIO_SetBits(DS18B20_PORT, DS18B20_PIN);
            __NOP();__NOP();
        }
    }
}

// DS18B20启动温度转换
void DS18B20_Start(void)
{
    DS18B20_Reset();
    DS18B20_Check();
    DS18B20_Write_Byte(0xCC);  // 跳过ROM
    DS18B20_Write_Byte(0x44);  // 启动转换
}

// 初始化DS18B20
uint8_t DS18B20_Init(void)
{
    // 使能端口时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    DS18B20_Reset();
    return DS18B20_Check();
}

// 读取温度
float DS18B20_ReadTemperture(void)
{
    float temp;
    uint8_t dath = 0, datl = 0;
    uint16_t value = 0;
    DS18B20_Start();
    DS18B20_Reset();
    DS18B20_Check();
    DS18B20_Write_Byte(0xCC);  // 跳过ROM
    DS18B20_Write_Byte(0xBE);  // 读取寄存器
    datl = DS18B20_Read_Byte();  // 低8位
    dath = DS18B20_Read_Byte();  // 高8位
    value = (dath << 8) + datl;
    // 温度计算（负温度处理）
    if((value & 0xF800) == 0xF800)
    {
        value = (~value) + 1;
        temp = value * (-0.0625);
    }
    else
    {
        temp = value * 0.0625;
    }
    return temp;
}
