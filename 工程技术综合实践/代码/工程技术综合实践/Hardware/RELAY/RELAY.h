#ifndef __RELAY_H
#define __RELAY_H

#define RELAY_PORT GPIOA
#define RELAY_PIN  GPIO_Pin_3

void RELAY_Init(void);
void RELAY_ON(void);
void RELAY_OFF(void);

#endif
