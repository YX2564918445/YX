#ifndef __KEY_H
#define __KEY_H

// 阈值变量（默认初始值）
extern uint16_t smoke_threshold;  // 烟雾阈值（0-100）
extern uint8_t temp_threshold;   // 温度阈值（0-100）

// 设置模式标志位（0=正常监测模式，1=对应阈值设置模式）
extern uint8_t smoke_set_mode;   // 烟雾阈值设置模式
extern uint8_t temp_set_mode;    // 温度阈值设置模式

// 函数声明
void Key_Init(void);       // 按键初始化（GPIO+EXTI+NVIC配置）
void Key_Scan(void);       // 按键扫描（处理加减键，修改阈值）

#endif