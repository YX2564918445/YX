#include "debug.h"
#include "OLED.h"
#include "BEEP.h"
#include "RELAY.h"
#include "LED.h"
#include "KEY.h"
#include "DS18B20.h"
#include "MQ_2.h"
/* Global typedef */
/* Global define */
/* Global Variable */
uint16_t Gas;
float temperature;
int main(void)
{
    // 模块初始化
    SystemCoreClockUpdate();
    Delay_Init();
    OLED_Init();                    // OLED初始化
    BEEP_Init();                    // 蜂鸣器初始化
    RELAY_Init();                   // 继电器初始化 
    LED_Init();                     // 指示灯初始化
    Key_Init();                     // 按键初始化
    MQ_2_Init();                    // MQ_2初始化
    DS18B20_Init();                 // DS18B20初始化
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    // OELD显示初始化
    OLED_Clear();
    OLED_ShowString(1,1,"Smog:    ppm");
    OLED_ShowString(2,1,"T:    .   ^C");
    OLED_ShowString(3,1,"RELAY:");
    OLED_ShowString(4,1,"S:     ");  // 显示烟雾阈值
    OLED_ShowString(4,8,"T:    ");  // 显示温度阈值
    
    while(1)
    {
        // 获取当前气体浓度和温度
        Gas = MQ2_GasGet();
        temperature = DS18B20_ReadTemperture();
        OLED_ShowNum(1,6,(uint32_t)Gas,4);                  // 显示当前气体浓度
        if(temperature < 0)
        {
            OLED_ShowString(2,3,"-");
        }
        else
        {
            OLED_ShowString(2,3," ");
        }
        OLED_ShowNum(2,4,(uint32_t)temperature,3);          // 显示当前温度整数部分
        OLED_ShowNum(2,8,(uint32_t)((temperature-(uint32_t)temperature)*1000),3);   // 显示温度小数部分
        Key_Scan();                                         // 按键扫描
        OLED_ShowNum(4,3,(uint32_t)smoke_threshold,4);      // 显示烟雾阈值
        OLED_ShowNum(4,10,(uint32_t)temp_threshold,3);      // 显示温度阈值
        // 超过阈值
        if(Gas >= smoke_threshold || temperature >= (float)temp_threshold)
        {
            RELAY_OFF();            // 继电器关闭
            LED_Overrun();          // 指示灯改变
            BEEP_ON();              // 蜂鸣器开启
        }
        else
        {
            RELAY_ON();            // 继电器管开
            LED_Normal();          // 指示灯改变
            BEEP_OFF();            // 蜂鸣器关闭
        }
    }
}
