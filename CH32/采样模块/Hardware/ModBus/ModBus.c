#include "modbus.h"
#include "debug.h"  // 确保包含USART头文件
#include "ch32v20x_gpio.h"
#include "ch32v20x_rcc.h"
#include <string.h>

// 全局变量定义
ModBusData mb_data = {0};
ModBusFrame mb_frame = {
    .slave_addr = MODBUS_SLAVE_ADDR,
    .rx_len = 0,
    .tx_len = 0
};

// 定时器配置（用于超时检测，使用TIM2）
static void ModBus_TimerInit(void) {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    TIM_TimeBaseInitTypeDef TIM_InitStruct;
    TIM_InitStruct.TIM_Period = 1000 - 1;  // 1ms计数（72MHz主频下）
    TIM_InitStruct.TIM_Prescaler = 72 - 1;
    TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_InitStruct);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    NVIC_EnableIRQ(TIM2_IRQn);
    TIM_Cmd(TIM2, ENABLE);
}

// UART配置（使用USART1，PA9-TX，PA10-RX）
static void ModBus_USARTInit(void) {  // 函数名同步修改为USART
    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;  // 结构体改为USART前缀
    NVIC_InitTypeDef NVIC_InitStruct;

    // 使能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    // 配置TX引脚（PA9）
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 配置RX引脚（PA10）
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 配置USART参数（所有成员改为USART前缀）
    USART_InitStruct.USART_BaudRate = MODBUS_BAUDRATE;
    USART_InitStruct.USART_WordLength = MODBUS_DATA_BITS;
    USART_InitStruct.USART_StopBits = MODBUS_STOP_BITS;
    USART_InitStruct.USART_Parity = MODBUS_PARITY;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART1, &USART_InitStruct);  // 初始化函数改为USART_Init

    // 使能接收中断（USART前缀）
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    // 配置中断优先级
    NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    USART_Cmd(USART1, ENABLE);
}

// ModBus初始化
void ModBus_Init(void) {
    ModBus_USARTInit();  // 调用修改后的USART初始化函数
    ModBus_TimerInit();
    // 初始化数据存储区
    memset(&mb_data, 0, sizeof(ModBusData));
}

// 计算ModBus CRC16校验
uint16_t ModBus_CRC16(uint8_t *buf, uint16_t len) {
    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < len; i++) {
        crc ^= buf[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc >>= 1;
                crc ^= 0xA001;  // 多项式
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;  // 高低位已交换，直接存储
}

// 处理读保持寄存器（功能码0x03）
static uint8_t ModBus_HandleReadHoldReg(uint8_t *rx, uint8_t *tx) {
    uint16_t start_addr = (rx[2] << 8) | rx[3];
    uint16_t reg_count = (rx[4] << 8) | rx[5];
    uint8_t tx_len = 3 + reg_count * 2;  // 地址+功能码+字节数+数据

    // 检查地址合法性
    if (start_addr + reg_count > HOLD_REG_MAX_COUNT || reg_count == 0) {
        return MB_ERR_ILLEGAL_ADDR;
    }

    // 组织响应帧
    tx[0] = mb_frame.slave_addr;
    tx[1] = MB_FUNC_READ_HOLD_REG;
    tx[2] = reg_count * 2;  // 数据字节数

    // 填充寄存器数据
    for (uint16_t i = 0; i < reg_count; i++) {
        tx[3 + i * 2] = (mb_data.hold_reg[start_addr + i] >> 8) & 0xFF;  // 高8位
        tx[4 + i * 2] = mb_data.hold_reg[start_addr + i] & 0xFF;         // 低8位
    }

    mb_frame.tx_len = tx_len;
    return MB_ERR_NONE;
}

// 处理写单个寄存器（功能码0x06）
static uint8_t ModBus_HandleWriteReg(uint8_t *rx, uint8_t *tx) {
    uint16_t reg_addr = (rx[2] << 8) | rx[3];
    uint16_t reg_val = (rx[4] << 8) | rx[5];

    // 检查地址合法性
    if (reg_addr >= HOLD_REG_MAX_COUNT) {
        return MB_ERR_ILLEGAL_ADDR;
    }

    // 写入寄存器
    mb_data.hold_reg[reg_addr] = reg_val;

    // 响应帧（回发原请求帧的前6字节）
    memcpy(tx, rx, 6);
    mb_frame.tx_len = 6;
    return MB_ERR_NONE;
}

// 通用错误响应处理
static void ModBus_SendError(uint8_t func_code, uint8_t err_code) {
    mb_frame.tx_buf[0] = mb_frame.slave_addr;
    mb_frame.tx_buf[1] = func_code | 0x80;  // 错误功能码（原功能码+0x80）
    mb_frame.tx_buf[2] = err_code;
    mb_frame.tx_len = 3;
}

// 处理接收帧
void ModBus_ProcessFrame(void) {
    uint8_t err_code = MB_ERR_NONE;
    uint16_t crc_calc, crc_recv;

    // 检查帧长度合法性
    if (mb_frame.rx_len < 4) {  // 最小帧：地址+功能码+数据（2字节）+CRC（2字节）
        goto frame_clear;
    }

    // 检查CRC校验
    crc_recv = (mb_frame.rx_buf[mb_frame.rx_len - 1] << 8) | mb_frame.rx_buf[mb_frame.rx_len - 2];
    crc_calc = ModBus_CRC16(mb_frame.rx_buf, mb_frame.rx_len - 2);
    if (crc_calc != crc_recv) {
        goto frame_clear;
    }

    // 检查从机地址
    if (mb_frame.rx_buf[0] != mb_frame.slave_addr && mb_frame.rx_buf[0] != 0xFF) {
        goto frame_clear;
    }

    // 处理功能码
    switch (mb_frame.rx_buf[1]) {
        case MB_FUNC_READ_HOLD_REG:
            err_code = ModBus_HandleReadHoldReg(mb_frame.rx_buf, mb_frame.tx_buf);
            break;
        case MB_FUNC_WRITE_REG:
            err_code = ModBus_HandleWriteReg(mb_frame.rx_buf, mb_frame.tx_buf);
            break;
        // 可根据需要添加其他功能码处理（0x01/0x02/0x04/0x05/0x0F/0x10等）
        default:
            err_code = MB_ERR_ILLEGAL_FUNC;
            break;
    }

    // 发送响应
    if (err_code == MB_ERR_NONE) {
        // 添加CRC到发送帧
        uint16_t crc = ModBus_CRC16(mb_frame.tx_buf, mb_frame.tx_len);
        mb_frame.tx_buf[mb_frame.tx_len++] = crc & 0xFF;
        mb_frame.tx_buf[mb_frame.tx_len++] = (crc >> 8) & 0xFF;
        ModBus_SendFrame();
    } else {
        ModBus_SendError(mb_frame.rx_buf[1], err_code);
        uint16_t crc = ModBus_CRC16(mb_frame.tx_buf, mb_frame.tx_len);
        mb_frame.tx_buf[mb_frame.tx_len++] = crc & 0xFF;
        mb_frame.tx_buf[mb_frame.tx_len++] = (crc >> 8) & 0xFF;
        ModBus_SendFrame();
    }

frame_clear:
    // 清空接收缓冲区
    mb_frame.rx_len = 0;
    memset(mb_frame.rx_buf, 0, MODBUS_MAX_FRAME_LEN);
}

// 发送帧
void ModBus_SendFrame(void) {
    for (uint16_t i = 0; i < mb_frame.tx_len; i++) {
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);  // USART前缀
        USART_SendData(USART1, mb_frame.tx_buf[i]);
    }
    // 发送完成后清空发送缓冲区
    mb_frame.tx_len = 0;
    memset(mb_frame.tx_buf, 0, MODBUS_MAX_FRAME_LEN);
}

// 超时检查（需在定时器中断中调用）
void ModBus_TimeoutCheck(void) {
    static uint32_t timeout_cnt = 0;
    if (mb_frame.rx_len > 0) {
        timeout_cnt++;
        if (timeout_cnt >= MODBUS_TIMEOUT_MS) {
            // 超时，处理接收帧
            ModBus_ProcessFrame();
            timeout_cnt = 0;
        }
    } else {
        timeout_cnt = 0;
    }
}

// USART1中断服务程序
void USART1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USART1_IRQHandler(void) {
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {  // USART前缀
        // 接收数据
        uint8_t data = USART_ReceiveData(USART1);
        if (mb_frame.rx_len < MODBUS_MAX_FRAME_LEN) {
            mb_frame.rx_buf[mb_frame.rx_len++] = data;
        }
        // 重置超时计数
        mb_frame.last_rx_time = TIM2->CNT;
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);  // USART前缀
    }
}

// TIM2中断服务程序（用于超时检测）
void TIM2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM2_IRQHandler(void) {
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
        ModBus_TimeoutCheck();
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}