#ifndef _DS18B20_H
#define _DS18B20_H

// Òý½Å¶¨Òåuint8_t
#define DS18B20_PORT    GPIOA
#define DS18B20_PIN     GPIO_Pin_7

void DS18B20_Set_Output(void);
void DS18B20_Set_Input(void);
void DS18B20_Reset(void);
uint8_t DS18B20_Check(void);
uint8_t DS18B20_Read_Bit(void);
uint8_t DS18B20_Read_Byte(void);
void DS18B20_Write_Byte(uint8_t dat);
void DS18B20_Start(void);
uint8_t DS18B20_Init(void);
float DS18B20_ReadTemperture(void);

#endif