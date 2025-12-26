#ifndef __MQ_2_H
#define __MQ_2_H

// 定义MQ_2的引脚
#define MQ2_PORT GPIOA
#define MQ2_PIN  GPIO_Pin_0

void MQ_2_Init(void);
uint16_t MQ2_ReadRaw(void);
float MQ2_ReadVoltage(void);
uint16_t MQ2_GasGet(void);

#endif
