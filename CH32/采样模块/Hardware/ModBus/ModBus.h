#ifndef __MODBUS_H
#define __MODBUS_H

#include "debug.h"

// ModBus配置参数
#define MODBUS_SLAVE_ADDR     0x01        // 从机地址
#define MODBUS_BAUDRATE       9600        // 波特率
#define MODBUS_DATA_BITS      USART_WordLength_8b  // 数据位（CH32用USART前缀）
#define MODBUS_STOP_BITS      USART_StopBits_1     // 停止位
#define MODBUS_PARITY         USART_Parity_Even    // 校验位（ModBus RTU常用偶校验）

// 最大帧长度（含地址、功能码、数据、CRC）
#define MODBUS_MAX_FRAME_LEN  256
// 超时时间（单位：ms，需大于3.5个字符时间，9600波特率下约5ms）
#define MODBUS_TIMEOUT_MS     10

// ModBus功能码
#define MB_FUNC_READ_COILS       0x01  // 读线圈状态
#define MB_FUNC_READ_DISCRETE    0x02  // 读离散输入
#define MB_FUNC_READ_HOLD_REG    0x03  // 读保持寄存器
#define MB_FUNC_READ_INPUT_REG   0x04  // 读输入寄存器
#define MB_FUNC_WRITE_COIL       0x05  // 写单个线圈
#define MB_FUNC_WRITE_REG        0x06  // 写单个寄存器
#define MB_FUNC_WRITE_MUL_COILS  0x0F  // 写多个线圈
#define MB_FUNC_WRITE_MUL_REGS   0x10  // 写多个寄存器

// ModBus错误码
#define MB_ERR_NONE             0x00  // 无错误
#define MB_ERR_ILLEGAL_FUNC     0x01  // 非法功能码
#define MB_ERR_ILLEGAL_ADDR     0x02  // 非法数据地址
#define MB_ERR_ILLEGAL_DATA     0x03  // 非法数据值
#define MB_ERR_SLAVE_FAIL       0x04  // 从机设备故障

// 线圈/离散输入最大数量（可根据实际需求调整）
#define COILS_MAX_COUNT         128
#define DISCRETE_MAX_COUNT      128

// 保持寄存器/输入寄存器最大数量（16位）
#define HOLD_REG_MAX_COUNT      64
#define INPUT_REG_MAX_COUNT     64

// 数据存储结构体
typedef struct {
    uint8_t  coils[COILS_MAX_COUNT / 8 + 1];       // 线圈状态（bit存储）
    uint8_t  discrete[DISCRETE_MAX_COUNT / 8 + 1]; // 离散输入（bit存储）
    uint16_t hold_reg[HOLD_REG_MAX_COUNT];         // 保持寄存器（16位）
    uint16_t input_reg[INPUT_REG_MAX_COUNT];       // 输入寄存器（16位）
} ModBusData;

// 帧处理结构体
typedef struct {
    uint8_t  rx_buf[MODBUS_MAX_FRAME_LEN];  // 接收缓冲区
    uint16_t rx_len;                        // 接收长度
    uint8_t  tx_buf[MODBUS_MAX_FRAME_LEN];  // 发送缓冲区
    uint16_t tx_len;                        // 发送长度
    uint8_t  slave_addr;                    // 从机地址
    uint32_t last_rx_time;                  // 最后接收时间（用于超时判断）
} ModBusFrame;

// 外部变量声明
extern ModBusData mb_data;
extern ModBusFrame mb_frame;

// 函数声明
void ModBus_Init(void);                           // 初始化ModBus
uint16_t ModBus_CRC16(uint8_t *buf, uint16_t len); // 计算CRC16
void ModBus_ProcessFrame(void);                    // 处理接收帧
void ModBus_SendFrame(void);                       // 发送帧
void ModBus_TimeoutCheck(void);                    // 超时检查（需定时调用）

#endif