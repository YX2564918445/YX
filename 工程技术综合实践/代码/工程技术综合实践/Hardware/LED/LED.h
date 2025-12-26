#ifndef __LED_H
#define __LED_H

#define LED_PORT GPIOA
#define RED_LED_PIN  GPIO_Pin_1
#define GREEN_LED_PIN  GPIO_Pin_2

void LED_Init(void);
void LED_Normal(void);
void LED_Overrun(void);

#endif
