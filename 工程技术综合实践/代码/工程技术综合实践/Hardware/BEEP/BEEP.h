#ifndef __BEEP_H
#define __BEEP_H

#define BEEP_PORT GPIOA
#define BEEP_PIN  GPIO_Pin_6

void BEEP_Init(void);
void BEEP_OFF(void);
void BEEP_ON(void);

#endif
