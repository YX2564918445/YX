#include "debug.h"
#include "LED.h"

/*ºì¡¢ÂÌÖ¸Ê¾µÆ³õÊ¼»¯*/
void LED_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = RED_LED_PIN|GREEN_LED_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED_PORT,&GPIO_InitStructure);
    GPIO_SetBits(LED_PORT,RED_LED_PIN);
    GPIO_SetBits(LED_PORT,GREEN_LED_PIN);
}

/*Õý³£Çé¿öLED*/
/*ÂÌµÆÁÁ¡¢ºìµÆÃð*/
void LED_Normal(void)
{
    GPIO_ResetBits(LED_PORT,GREEN_LED_PIN);
    GPIO_SetBits(LED_PORT,RED_LED_PIN);
}

/*³¬¹ý·¶Î§LED*/
/*ºìµÆÁÁ¡¢ÂÌµÆÃð*/
void LED_Overrun(void)
{
    GPIO_ResetBits(LED_PORT,RED_LED_PIN);
    GPIO_SetBits(LED_PORT,GREEN_LED_PIN);
}
