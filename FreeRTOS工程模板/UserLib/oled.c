#include "oled.h"
#include "delay.h"
#include "oled_font.h"


void OLED_Init(void)
{
    delay_s(1);             //延时1s，稳定OLED上电状态
    OLED_I2C_Init();			//端口初始化
    
	
    /* 直接复制粘贴以下命令 */

	OLED_WriteCmd(0xAE);	//关闭显示
	
	OLED_WriteCmd(0xD5);	//设置显示时钟分频比/振荡器频率
	OLED_WriteCmd(0x80);
	
	OLED_WriteCmd(0xA8);	//设置多路复用率
	OLED_WriteCmd(0x3F);
	
	OLED_WriteCmd(0xD3);	//设置显示偏移
	OLED_WriteCmd(0x00);
	
	OLED_WriteCmd(0x40);	//设置显示开始行
	
	OLED_WriteCmd(0xA1);	//设置左右方向，0xA1正常 0xA0左右反置
	
	OLED_WriteCmd(0xC8);	//设置上下方向，0xC8正常 0xC0上下反置

	OLED_WriteCmd(0xDA);	//设置COM引脚硬件配置
	OLED_WriteCmd(0x12);
	
	OLED_WriteCmd(0x81);	//设置对比度控制
	OLED_WriteCmd(0xCF);

	OLED_WriteCmd(0xD9);	//设置预充电周期
	OLED_WriteCmd(0xF1);

	OLED_WriteCmd(0xDB);	//设置VCOMH取消选择级别
	OLED_WriteCmd(0x30);

	OLED_WriteCmd(0xA4);	//设置整个显示打开/关闭

	OLED_WriteCmd(0xA6);	//设置正常/倒转显示

	OLED_WriteCmd(0x8D);	//设置充电泵
	OLED_WriteCmd(0x14);

	OLED_WriteCmd(0xAF);	//开启显示

    OLED_Clear();
}

void OLED_I2C_Start(void)
{
    SDA_WriteBit(1);
    SCL_WriteBit(1);
    SDA_WriteBit(0);
    SCL_WriteBit(0);    //SCL为0，SCL的下一个时钟脉冲时开始发送数据
}

void OLED_I2C_Stop(void)
{
    SDA_WriteBit(0);
    SCL_WriteBit(1);
    SDA_WriteBit(1);
}

/**
 *  @brief  软件模拟iic初始化
 *  @param  无
 *  @retval 无
 * 
 */

void OLED_I2C_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;    //开漏输出模式，只是有在低电平下采用驱动能力
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	SCL_WriteBit(1);        //拉高SCL和SDA
	SDA_WriteBit(1);
}

void OLED_SendByte(uint8_t byte)
{
    for(uint8_t i = 0; i < 8; i++)
    {
        SDA_WriteBit(byte & ((0x80) >> i));     //依次发送byte中每一位二进制数据
        SCL_WriteBit(1);
        SCL_WriteBit(0);
    }
    SCL_WriteBit(1);	//额外的一个时钟，不处理应答信号
	SCL_WriteBit(0);
}

/**
 * @brief  OLED显示一个字符
 * @param  row 写入字符的行号 取值1~4
 * @param  col 写入的列号     取值1~16  
 * @param  ch  要写入的字符
 * @retval 无
 * 
 * 0~7 =====>  1~4
 * 输入1，处理0,1
 * 输入2，处理2,3
 * 输入3，处理4,5
 * 输入4，输出6,7
 * 
 * (1, 0)
 * y = kx+b 过(1, 0), (2, 2)解出y = 2x-2 = 2*(x - 1)
 * 
 */
void OLED_ShowChar(uint8_t row, uint8_t col, uint8_t ch)
{
    OLED_SetCursor((row - 1) * 2, (col - 1)* 8);
    for (int i = 0; i < 8; i++)
    {
        OLED_WriteData(OLED_F8x16[ch - ' '][i]);
    }
    OLED_SetCursor((row - 1) * 2 + 1, (col - 1)* 8);
    for (int i = 0; i < 8; i++)
    {
        OLED_WriteData(OLED_F8x16[ch - ' '][i + 8]);
    }
}

void OLED_ShowString(uint8_t row, uint8_t col, uint8_t *str)
{
    for (uint8_t i = 0; str[i] != '\0'; i++)
    {
        OLED_ShowChar(row, col + i, str[i]);
    }
    
}

/**
  * @brief  OLED写指令
  * @param  cmd 要写入的数据
  * @retval 无
  * 
  * *******************************************************
  * 从机地址：7位+1位读写位，D/C#引脚充当SA0位，SA0=1，数据模式，SA0=0，指令模式
  *          R/W#决定是读还是写操作，1是读，0是写
  *         ┌────┬────┬────┬────┬────┬────┬────┬────┐
  *         │ b7 │ b6 │ b5 │ b4 │ b3 │ b2 │ b1 │ b0 │
  *         ├────┼────┼────┼────┼────┼────┼────┼────┤
  *         │ 0  │ 1  │ 1  │ 1  │ 1  │ 0  │SA0 │R/W#│
  *         └────┴────┴────┴────┴────┴────┴────┴────┘  
  * 
  * 控制字节：
  *         Co位： If the Co bit is set as logic “0”, the transmission of the following information will contain data bytes only.  
  *         D/C#： The D/C# bit determines the next data byte is acted as a command or a data. If the D/C# bit is 
  *         set to logic “0”, it defines the following data byte as a command. If the D/C# bit is set to 
  *         logic “1”, it defines the following data byte as a data which will be stored at the GDDRAM. 
  *         The GDDRAM column address pointer will be increased by one automatically after each 
  *         data write. 
  *         ┌────┬────┬────┬────┬────┬────┬────┬────┬───┐
  *         │ b7 │ b6 │ b5 │ b4 │ b3 │ b2 │ b1 │ b0 │   │
  *         ├────┼────┼────┼────┼────┼────┼────┼────┤ACK│
  *         │ Co │D/C#│ 0  │ 0  │ 0  │ 0  │ 0  │ 0  │   │
  *         └────┴────┴────┴────┴────┴────┴────┴────┴───┘
  *  
  * *********************************************************
  * 
  */
void OLED_WriteCmd(uint8_t cmd)
{
    OLED_I2C_Start();
    OLED_SendByte(0x78);    //地址字节：发送slave(从机)地址
    OLED_SendByte(0X00);    //控制字节：准备写指令，Co = 0, D/C# = 0
    OLED_SendByte(cmd);     //写指令
    OLED_I2C_Stop();
}

void OLED_WriteData(uint8_t data)
{
    OLED_I2C_Start();
    OLED_SendByte(0x78);    //地址字节：发送slave(从机)地址
    OLED_SendByte(0X40);    //控制字节：准备写数据，Co = 0, D/C# = 1
    OLED_SendByte(data);    //写数据
    OLED_I2C_Stop();
}


/**
  * @brief  OLED设置光标位置
  * @param  Y 以左上角为原点，向下方向的坐标，范围：0~7
  * @param  X 以左上角为原点，向右方向的坐标，范围：0~127
  * @retval 无
  * 
  * **************************************************
  *     128*64的点阵，从行方面来看64行的点阵分成为了8个PAGE，即PAGE0~PAGE7，这也是为什么y取0~7
  *     从列方面来看，128列的点阵，即SEG0~SGE127，所以x取值0~127
  * 
  */
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
	OLED_WriteCmd(0xB0 | Y);					//设置PAGE起始地址
	OLED_WriteCmd(0x10 | ((X & 0xF0) >> 4));	//取x的高4位并右移4位，与固定的0x10按位或，设置列地址高4位
	OLED_WriteCmd(0x00 | (X & 0x0F));			//取x的低4位设置列地址低4位，与固定的0x00按位或，设置列地址低4位
}

/* OLED清屏  */
void OLED_Clear(void)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        OLED_SetCursor(i, 0);   //光标设置到第i行第1列
        for (uint8_t j  = 0; j < 128; j++)
        {
            OLED_WriteData(0X00);
        } 
    }
}
