#include "debug.h"
#include "RELAY.h"
#include "OLED.h"

/*继电器初始化*/
void RELAY_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = RELAY_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(RELAY_PORT,&GPIO_InitStructure);
}

/*继电器开启,并显示当前状态*/
void RELAY_ON(void)
{
    GPIO_SetBits(RELAY_PORT,RELAY_PIN);
    OLED_ShowString(3,7," OK");
}

/*继电器关闭，并显示当前状态*/
void RELAY_OFF(void)
{
    GPIO_ResetBits(RELAY_PORT,RELAY_PIN);
    OLED_ShowString(3,7," NO");
}
