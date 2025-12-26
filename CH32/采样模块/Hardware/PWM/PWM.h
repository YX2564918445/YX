#ifndef __PWM_H
#define __PWM_H

#define PWM1_PORT GPIOA
#define PWM1_PIN  GPIO_Pin_6
#define PWM2_PORT GPIOB
#define PWM2_PIN  GPIO_Pin_6

// 初始化TIM3_CH1(PA6)
void TIM3_PWM_Init(uint16_t arr, uint16_t psc, uint16_t ccr);
// 初始化TIM4_CH1(PB6)
void TIM4_PWM_Init(uint16_t arr, uint16_t psc, uint16_t ccr);

#endif
