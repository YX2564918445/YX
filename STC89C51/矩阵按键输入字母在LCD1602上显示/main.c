#include <reg52.h>
#include <intrins.h>

sbit RS = P2^6;
sbit RW = P2^5;
sbit EN = P2^7;
#define LCD_DATA P0
#define KEY_MATRIX_PORT P1

char display_buffer[32] = "                                ";
unsigned char buffer_index = 0;

void delay_ms(unsigned int ms) {
    unsigned int i, j;
    for(i = 0; i < ms; i++)
        for(j = 0; j < 114; j++);
}

void lcd_write_cmd(unsigned char cmd) {
    RS = 0;
    RW = 0;
    LCD_DATA = cmd;
    EN = 1;
    _nop_();
    _nop_();
    EN = 0;
    delay_ms(2);
}

void lcd_write_data(unsigned char dat) {
    RS = 1;
    RW = 0;
    LCD_DATA = dat;
    EN = 1;
    _nop_();
    _nop_();
    EN = 0;
    delay_ms(2);
}

void lcd_init() {
    delay_ms(15);
    lcd_write_cmd(0x38);
    delay_ms(5);
    lcd_write_cmd(0x38);
    delay_ms(5);
    lcd_write_cmd(0x38);
    delay_ms(5);
    lcd_write_cmd(0x0C);
    delay_ms(5);
    lcd_write_cmd(0x06);
    delay_ms(5);
    lcd_write_cmd(0x01);
    delay_ms(5);
}

void update_lcd_display() {
    unsigned char i;
    lcd_write_cmd(0x80);
    for(i = 0; i < 16; i++) {
        lcd_write_data(display_buffer[i]);
    }
    lcd_write_cmd(0xC0);
    for(i = 16; i < 32; i++) {
        lcd_write_data(display_buffer[i]);
    }
}

// 修改后的缓冲区更新函数
void update_display_buffer(char new_char) {
    // 当缓冲区满时，整体上移一行
    if (buffer_index >= 32) {
        unsigned char i;
        // 第一行用第二行覆盖
        for (i = 0; i < 16; i++) {
            display_buffer[i] = display_buffer[i+16];
        }
        // 第二行清空
        for (i = 16; i < 32; i++) {
            display_buffer[i] = ' ';
        }
        buffer_index = 16; // 从第二行开始
    }
    
    // 直接写入当前索引位置
    display_buffer[buffer_index] = new_char;
    buffer_index++;
    
    update_lcd_display();
}

unsigned char get_key() {
    unsigned char key_value = 0;
    KEY_MATRIX_PORT = 0xf7;
    if (KEY_MATRIX_PORT != 0xf7) {
        delay_ms(10);
        switch (KEY_MATRIX_PORT) {
            case 0x77: key_value = 1; break;
            case 0xb7: key_value = 5; break;
            case 0xd7: key_value = 9; break;
            case 0xe7: key_value = 13; break;
        }
    }
    while (KEY_MATRIX_PORT != 0xf7);
    
    KEY_MATRIX_PORT = 0xfb;
    if (KEY_MATRIX_PORT != 0xfb) {
        delay_ms(10);
        switch (KEY_MATRIX_PORT) {
            case 0x7b: key_value = 2; break;
            case 0xbb: key_value = 6; break;
            case 0xdb: key_value = 10; break;
            case 0xeb: key_value = 14; break;
        }
    }
    while (KEY_MATRIX_PORT != 0xfb);
    
    KEY_MATRIX_PORT = 0xfd;
    if (KEY_MATRIX_PORT != 0xfd) {
        delay_ms(10);
        switch (KEY_MATRIX_PORT) {
            case 0x7d: key_value = 3; break;
            case 0xbd: key_value = 7; break;
            case 0xdd: key_value = 11; break;
            case 0xed: key_value = 15; break;
        }
    }
    while (KEY_MATRIX_PORT != 0xfd);
    
    KEY_MATRIX_PORT = 0xfe;
    if (KEY_MATRIX_PORT != 0xfe) {
        delay_ms(10);
        switch (KEY_MATRIX_PORT) {
            case 0x7e: key_value = 4; break;
            case 0xbe: key_value = 8; break;
            case 0xde: key_value = 12; break;
            case 0xee: key_value = 16; break;
        }
    }
    while (KEY_MATRIX_PORT != 0xfe);
    
    return key_value - 1;
}

void main() {
    unsigned char key;
    unsigned char i;
    
    lcd_init();
    for (i = 0; i < 32; i++) {
        display_buffer[i] = ' ';
    }
    buffer_index = 0;
    update_lcd_display();
    
    while (1) {
        key = get_key();
        if (key != 0xFF) {
            char letter = 'A' + key;
            update_display_buffer(letter);
        }
    }
}