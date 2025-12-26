# include "reg52.h"
// 引脚定义
#define KEY_MATRIX_PORT	P1		// 宏定义矩阵按键控制口		
#define SMG_A_DP_PORT	P0		// 宏定义数码管段码口
sbit LSA=P2^2;					// 定义数码管位选控制端口
sbit LSB=P2^3;
sbit LSC=P2^4;
// 变量定义
unsigned char smg_code[18]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
				0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71,0X00};	// 共阴极数码管显示0~F的段码数据
unsigned char dis_nuber[8] = {17,17,17,17,17,17,17,17,}  ;      // 当前显示的八个键盘标号
unsigned int i = 0 ; 											// 循环变量定义
unsigned char key_number ;  									// 按下按键标号

// 毫秒级延迟函数
void delay_ms(unsigned int time) 
{
	unsigned int j , i;

	for( i = 0; i < time; i++)

		for( j = 0; j < 144 ; j++);
}

// 矩阵按键扫描函数
int key_matrix_ranks_scan(void)
{
	unsigned char key_value=0;
	KEY_MATRIX_PORT=0XF7;				// 给第一列赋值0，其余全为1
	if(KEY_MATRIX_PORT!=0XF7)			// 判断第一列按键是否按下
	{
		delay_ms(10);					// 消抖
		switch(KEY_MATRIX_PORT)			// 保存第一列按键按下后的键值	
		{
			case 0X77: key_value=1;break;
			case 0XB7: key_value=5;break;
			case 0XD7: key_value=9;break;
			case 0XE7: key_value=13;break;
		}
	}
	while(KEY_MATRIX_PORT!=0xF7);		// 等待按键松开	
	KEY_MATRIX_PORT=0xFB;				// 给第二列赋值0，其余全为1
	if(KEY_MATRIX_PORT!=0xFB)			// 判断第二列按键是否按下
	{
		delay_ms(10);					// 消抖
		switch(KEY_MATRIX_PORT)			// 保存第二列按键按下后的键值	
		{
			case 0X7B: key_value=2;break;
			case 0XBB: key_value=6;break;
			case 0XDB: key_value=10;break;
			case 0XEB: key_value=14;break;
		}
	}
	while(KEY_MATRIX_PORT!=0XFB);		// 等待按键松开	
	KEY_MATRIX_PORT=0XFD;				// 给第三列赋值0，其余全为1
	if(KEY_MATRIX_PORT!=0XFD)			// 判断第三列按键是否按下
	{
		delay_ms(10);					// 消抖
		switch(KEY_MATRIX_PORT)			// 保存第三列按键按下后的键值	
		{
			case 0X7D: key_value=3;break;
			case 0XBD: key_value=7;break;
			case 0XDD: key_value=11;break;
			case 0XED: key_value=15;break;
		}
	}
	while(KEY_MATRIX_PORT!=0XFD);		// 等待按键松开	
	KEY_MATRIX_PORT=0XFE;				// 给第四列赋值0，其余全为1
	if(KEY_MATRIX_PORT!=0XFE)			// 判断第四列按键是否按下
	{
		delay_ms(10);					// 消抖
		switch(KEY_MATRIX_PORT)			// 保存第四列按键按下后的键值	
		{
			case 0X7E: key_value=4;break;
			case 0XBE: key_value=8;break;
			case 0XDE: key_value=12;break;
			case 0XEE: key_value=16;break;
		}
	}
	while(KEY_MATRIX_PORT!=0XFE);		// 等待按键松开
	return key_value;					// 返回按下的按键号
}

// 数码管显示函数
void display()
{
	for(i=0;i<8;i++)
	{
	   	switch(i)								//位选控制
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
		SMG_A_DP_PORT= smg_code[dis_nuber[i]];	//传送段选数据
		delay_ms(1);							//延时一段时间，等待显示稳定
		SMG_A_DP_PORT=0X00;						//消隐
	}
}
// 主函数
void main()
{
	while(1)
	{
		key_number = key_matrix_ranks_scan(); 	// 储存当前按下按键的标号
		if ( key_number != 0) 				  	// 有按键按下
		{
			for(i = 0;i < 7;i++)
				dis_nuber[i] = dis_nuber[i+1];	// 键值左移
			dis_nuber[7] = key_number - 1;		// 新键值放在最右边
		}
		display();								// 数码管显示
	}
}