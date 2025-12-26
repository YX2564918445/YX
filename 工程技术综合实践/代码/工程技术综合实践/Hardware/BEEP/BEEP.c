#include "debug.h"
#include "BEEP.h"

/*·äÃùÆ÷³õÊ¼»¯*/
void BEEP_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = BEEP_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BEEP_PORT,&GPIO_InitStructure);
}

/*·äÃùÆ÷¹Ø*/
void BEEP_OFF(void)
{
    GPIO_ResetBits(BEEP_PORT,BEEP_PIN);
}

/*·äÃùÆ÷¿ª*/
void BEEP_ON(void)
{
    GPIO_SetBits(BEEP_PORT,BEEP_PIN);
}
