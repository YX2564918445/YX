#include <reg52.h>
#include <stdlib.h>
#define uchar unsigned char
#define uint unsigned int

// 数码管位选控制 
sbit LSA = P2^2;  
sbit LSB = P2^3;  
sbit LSC = P2^4;  

// 全局变量定义
long num1 = 0, num2 = 0, result = 0;  // 输入数和结果
long current_value = 0;     // 当前输入值
uchar input_count = 0;      // 输入位数计数
uchar operation = 0;        // 运算符 (0:无, 1:+, 2:-, 3:*, 4:/)
uchar clear_flag = 0;       // 清除显示标志
uchar display_data[8];      // 数码管显示缓冲区

// 数码管段码表 
uchar code seg_table[] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66, // 0-4
    0x6D, 0x7D, 0x07, 0x7F, 0x6F, // 5-9
    0x77, 0x7C, 0x39, 0x5E, 0x79, // A, b, C, d, E
    0x71, 0x08, 0x40, 0x00        // F, _, - , 空
};

// 按键映射表 (
uchar code key_table[4][4] = {
    {15,  0, 14, 13},   //第四行: C(15), 0, =(14), +(13)
    { 1,  2,  3, 10},   // 第三行: 1, 2, 3, -(10)
    { 4,  5,  6, 11},   // 第二行: 4, 5, 6, *(11)
    { 7,  8,  9, 12}    // 第一行: 7, 8, 9, /(12)
};

// 毫秒级延时函数
void delay_ms(uint ms) {
    uint i, j;
    for(i = 0; i < ms; i++)
        for(j = 0; j < 114; j++);  
}

// 数码管显示函数
void display() {
    uchar i;
    for(i = 0; i < 8; i++) {
        // 位选控制 
        switch(i)
		{
			case 0: LSC=1;LSB=1;LSA=1;break;
			case 1: LSC=1;LSB=1;LSA=0;break;
			case 2: LSC=1;LSB=0;LSA=1;break;
			case 3: LSC=1;LSB=0;LSA=0;break;
			case 4: LSC=0;LSB=1;LSA=1;break;
			case 5: LSC=0;LSB=1;LSA=0;break;
			case 6: LSC=0;LSB=0;LSA=1;break;
			case 7: LSC=0;LSB=0;LSA=0;break;
		}
        // 段选输出
        P0 = seg_table[display_data[i]];
        delay_ms(1);         // 显示延时
        P0 = 0x00;           // 消隐 
    }
}

// 获取按键值 
uchar get_key() {
    uchar row, col;
    uchar key_value = 0xFF;
    uchar temp;
    
    // 扫描4行 (从物理底部行开始扫描)
    for(row = 0; row < 4; row++) {
        // 激活当前行(输出低电平)
        P1 = 0xFF & ~(0x10 << (3 - row));  // 行选择: 依次激活物理行
        // 读取列状态
        temp = P1 & 0x0F;     // 取低4位(列状态)
        // 检查是否有列被拉低
        if(temp != 0x0F) {
            // 消抖
            delay_ms(10);
            temp = P1 & 0x0F;
            if(temp != 0x0F) {
                // 确定被按下的列
                switch(temp) {
                    case 0x07: col = 0; break; // P1.3=0 (0111)
                    case 0x0B: col = 1; break; // P1.2=0 (1011)
                    case 0x0D: col = 2; break; // P1.1=0 (1101)
                    case 0x0E: col = 3; break; // P1.0=0 (1110)
                    default: break;
                }
                // 获取按键值 (物理行号需要反转)
                key_value = key_table[3 - row][col];
                // 等待按键释放
                while((P1 & 0x0F) != 0x0F);
                return key_value;
            }
        }
    }
    return 0xFF;
}

// 更新显示缓冲区 
void update_display() {
    long temp = current_value; // 始终显示当前值
    uchar i;
	uchar pos = 7; // 从最右边的数码管开始
    uchar digits[8];
    uchar num_digits = 0;
    uchar negative = 0;
    uchar start_pos = 0;  
    // 清除显示
    if(clear_flag) {
        for(i = 0; i < 8; i++) 
            display_data[i] = 16; // 16对应空显示
        clear_flag = 0;          // 清除标志复位
        return;
    }
    
    // 检查是否为负数
    if(temp < 0) {
        negative = 1;
        temp = -temp;
    }
    // 分离数字 (低位在前，高位在后)
    if(temp == 0) {
        // 输入0时显示0
        digits[0] = 0;
        num_digits = 1;
    } else {
        // 分离出所有数字（个位在前）
        while(temp > 0 && num_digits < 8) {
            digits[num_digits] = temp % 10;  // 先分离个位
            temp /= 10;
            num_digits++;
        }
    }
    // 1. 初始化所有显示为空
    for(i = 0; i < 8; i++) {
        display_data[i] = 16;
    }
    // 3. 放置数字（从个位开始，向左放置更高位）
    for(i = 0; i < num_digits; i++) {
        if(pos < 8) { // 确保不会越界
            display_data[pos] = digits[i];
            pos--;
        }
    }
    // 4. 放置负号（如果有空间）
    if(negative && pos >= 0) {
        display_data[pos] = 17; // 负号
    }
}

// 主函数 - 程序入口
void main() {
    uchar key;
    uchar i;
    // 初始化显示 - 全空
    for(i = 0; i < 8; i++) 
        display_data[i] = 16;
    while(1) {
        key = get_key();  // 获取按键
        if(key != 0xFF) { // 有按键按下
            // 数字键处理
            if(key <= 9) { 
                if(input_count < 8) { // 最多8位
                    // 计算新值
                    current_value = current_value * 10 + key;
                    input_count++;
                    // 更新显示
                    update_display();
                }
            } 
            // 清除键 (C)
            else if(key == 15) { 
                num1 = 0;
                num2 = 0;
                result = 0;
                current_value = 0;
                operation = 0;
                input_count = 0;
                clear_flag = 1; // 设置清除标志
                update_display();
            } 
            // 等于键 (=)
            else if(key == 14) { 
                if(operation) { // 有运算符时才计算
                    num2 = current_value; // 保存第二个操作数
                    // 执行运算
                    switch(operation) {
                        case 1: result = num1 + num2; break; // 加
                        case 2: result = num1 - num2; break; // 减
                        case 3: result = num1 * num2; break; // 乘
                        case 4: 
                            if(num2 != 0) result = num1 / num2; // 除
                            else result = 0; // 除0处理
                            break;
                    }
                    // 显示结果
                    current_value = result;
                    operation = 0;   // 运算符复位
                    num1 = result;   // 结果作为下一次的第一个操作数
                    input_count = 0; // 输入计数复位
                    // 更新显示
                    update_display();
                }
            } 
            // 运算符键 (+, -, *, /)
            else if(key >= 10 && key <= 13) { 
                // 保存第一个操作数
                if(operation == 0) {
                    num1 = current_value;
                }
                // 设置运算符
                if(key == 13) operation = 1;      // +
                else if(key == 10) operation = 2; // -
                else if(key == 11) operation = 3; // *
                else if(key == 12) operation = 4; // /
                // 准备输入新操作数
                current_value = 0;
                input_count = 0;
                // 清除显示
                clear_flag = 1;
                update_display();
                clear_flag = 0;
            }
        }
        display(); // 刷新数码管显示
    }
}