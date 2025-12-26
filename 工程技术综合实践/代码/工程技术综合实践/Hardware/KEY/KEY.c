#include "debug.h"
#include "key.h"
#include "LED.h"

// 阈值变量定义
uint16_t smoke_threshold = 1500;  // 烟雾阈值
uint8_t temp_threshold = 40;   // 温度阈值
// 设置模式标志位（默认0：正常模式）
uint8_t smoke_set_mode = RESET;
uint8_t temp_set_mode = RESET;

// 按键初始化
void Key_Init(void)
{
    // 开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
    // 配置GPIO
    GPIO_InitTypeDef GPIO_InitStructure;
    // 烟雾阈值设置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
    // 温度阈值设置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    // 阈值设定按键
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_3|GPIO_Pin_4;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_Init(GPIOC,&GPIO_InitStructure);
    // AFIO引脚选择
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource0);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource5);
    // EXTI初始化
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_Init(&EXTI_InitStructure);
    EXTI_InitStructure.EXTI_Line = EXTI_Line5;
    EXTI_Init(&EXTI_InitStructure);
    // NVIC配置
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_Init(&NVIC_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;  
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStructure);

}

// EXTI0中断服务函数声明
void EXTI0_IRQHandler(void) __attribute__((interrupt()));
// PB0（烟雾设置键）中断服务函数
void EXTI0_IRQHandler(void)
{
    Delay_Ms(10);  // 消抖
    if (EXTI_GetITStatus(EXTI_Line0) != RESET)  // 确认是EXTI0触发
    {
        smoke_set_mode = !smoke_set_mode;  // 切换模式（0→1进入，1→0退出）
        if (smoke_set_mode)
        {
            LED_Normal();
            temp_set_mode = 0;  // 进入烟雾设置时，关闭温度设置（避免同时设置）
        }
        else
        {
            LED_Overrun();
        }
    }
    EXTI_ClearITPendingBit(EXTI_Line0);  // 清除中断标志位
}

// EXTI9_5中断服务函数声明
void EXTI9_5_IRQHandler(void) __attribute__((interrupt()));
// PA5（温度设置键）中断服务函数
void EXTI9_5_IRQHandler(void)
{
    Delay_Ms(10);  // 消抖
    if (EXTI_GetITStatus(EXTI_Line5) != RESET)  // 确认是EXTI5触发
    {
        temp_set_mode = !temp_set_mode;  // 切换模式
        if (temp_set_mode)
        {
            LED_Overrun();
            smoke_set_mode = 0;  // 进入温度设置时，关闭烟雾设置
        }
        else
        {
            LED_Normal();
        }
        EXTI_ClearITPendingBit(EXTI_Line5);  // 清除中断标志位
    }
}

// 按键扫描
void Key_Scan(void)
{
    // 烟雾阈值加减（PB1=+，PC13=-）
    if (smoke_set_mode == 1)
    {
        // 烟雾+（PB1按下：下拉输入→低电平）
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == RESET)
        {
            Delay_Ms(10);  // 消抖
            if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == RESET)
            {
                smoke_threshold += 100;
                if (smoke_threshold > 10000)  // 阈值上限
                    smoke_threshold = 10000;
                while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == RESET);  // 等待按键释放
            }
        }

        // 烟雾-（PC13按下）
        if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == RESET)
        {
            Delay_Ms(10);  // 消抖
            if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == RESET)
            {
                if (smoke_threshold > 0)  // 阈值下限
                    smoke_threshold -= 100;
                else
                    smoke_threshold = 0;
                while (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == RESET);  // 等待释放
            }
        }
    }

    // 温度阈值加减（PB3=+，PB4=-）
    if (temp_set_mode == 1)
    {
        // 温度+（PB3按下）
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3) == RESET)
        {
            Delay_Ms(10);  // 消抖
            if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3) == RESET)
            {
                temp_threshold++;
                if (temp_threshold > 100)  // 阈值上限
                    temp_threshold = 100;
                while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3) == RESET);  // 等待释放
            }
        }

        // 温度-（PB4按下）
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) == RESET)
        {
            Delay_Ms(10);  // 消抖
            if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) == RESET)
            {
                if (temp_threshold > 0)  // 阈值下限
                    temp_threshold--;
                else
                    temp_threshold = 0;
                while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) == RESET);  // 等待释放
            }
        }
    }
}
