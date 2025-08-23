#include "oled.h"
#include "delay.h"
#include "oled_font.h"


void OLED_Init(void)
{
    delay_s(1);             //��ʱ1s���ȶ�OLED�ϵ�״̬
    OLED_I2C_Init();			//�˿ڳ�ʼ��
    
	
    /* ֱ�Ӹ���ճ���������� */

	OLED_WriteCmd(0xAE);	//�ر���ʾ
	
	OLED_WriteCmd(0xD5);	//������ʾʱ�ӷ�Ƶ��/����Ƶ��
	OLED_WriteCmd(0x80);
	
	OLED_WriteCmd(0xA8);	//���ö�·������
	OLED_WriteCmd(0x3F);
	
	OLED_WriteCmd(0xD3);	//������ʾƫ��
	OLED_WriteCmd(0x00);
	
	OLED_WriteCmd(0x40);	//������ʾ��ʼ��
	
	OLED_WriteCmd(0xA1);	//�������ҷ���0xA1���� 0xA0���ҷ���
	
	OLED_WriteCmd(0xC8);	//�������·���0xC8���� 0xC0���·���

	OLED_WriteCmd(0xDA);	//����COM����Ӳ������
	OLED_WriteCmd(0x12);
	
	OLED_WriteCmd(0x81);	//���öԱȶȿ���
	OLED_WriteCmd(0xCF);

	OLED_WriteCmd(0xD9);	//����Ԥ�������
	OLED_WriteCmd(0xF1);

	OLED_WriteCmd(0xDB);	//����VCOMHȡ��ѡ�񼶱�
	OLED_WriteCmd(0x30);

	OLED_WriteCmd(0xA4);	//����������ʾ��/�ر�

	OLED_WriteCmd(0xA6);	//��������/��ת��ʾ

	OLED_WriteCmd(0x8D);	//���ó���
	OLED_WriteCmd(0x14);

	OLED_WriteCmd(0xAF);	//������ʾ

    OLED_Clear();
}

void OLED_I2C_Start(void)
{
    SDA_WriteBit(1);
    SCL_WriteBit(1);
    SDA_WriteBit(0);
    SCL_WriteBit(0);    //SCLΪ0��SCL����һ��ʱ������ʱ��ʼ��������
}

void OLED_I2C_Stop(void)
{
    SDA_WriteBit(0);
    SCL_WriteBit(1);
    SDA_WriteBit(1);
}

/**
 *  @brief  ���ģ��iic��ʼ��
 *  @param  ��
 *  @retval ��
 * 
 */

void OLED_I2C_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;    //��©���ģʽ��ֻ�����ڵ͵�ƽ�²�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	SCL_WriteBit(1);        //����SCL��SDA
	SDA_WriteBit(1);
}

void OLED_SendByte(uint8_t byte)
{
    for(uint8_t i = 0; i < 8; i++)
    {
        SDA_WriteBit(byte & ((0x80) >> i));     //���η���byte��ÿһλ����������
        SCL_WriteBit(1);
        SCL_WriteBit(0);
    }
    SCL_WriteBit(1);	//�����һ��ʱ�ӣ�������Ӧ���ź�
	SCL_WriteBit(0);
}

/**
 * @brief  OLED��ʾһ���ַ�
 * @param  row д���ַ����к� ȡֵ1~4
 * @param  col д����к�     ȡֵ1~16  
 * @param  ch  Ҫд����ַ�
 * @retval ��
 * 
 * 0~7 =====>  1~4
 * ����1������0,1
 * ����2������2,3
 * ����3������4,5
 * ����4�����6,7
 * 
 * (1, 0)
 * y = kx+b ��(1, 0), (2, 2)���y = 2x-2 = 2*(x - 1)
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
  * @brief  OLEDдָ��
  * @param  cmd Ҫд�������
  * @retval ��
  * 
  * *******************************************************
  * �ӻ���ַ��7λ+1λ��дλ��D/C#���ų䵱SA0λ��SA0=1������ģʽ��SA0=0��ָ��ģʽ
  *          R/W#�����Ƕ�����д������1�Ƕ���0��д
  *         �����������Щ��������Щ��������Щ��������Щ��������Щ��������Щ��������Щ���������
  *         �� b7 �� b6 �� b5 �� b4 �� b3 �� b2 �� b1 �� b0 ��
  *         �����������੤�������੤�������੤�������੤�������੤�������੤�������੤��������
  *         �� 0  �� 1  �� 1  �� 1  �� 1  �� 0  ��SA0 ��R/W#��
  *         �����������ة��������ة��������ة��������ة��������ة��������ة��������ة���������  
  * 
  * �����ֽڣ�
  *         Coλ�� If the Co bit is set as logic ��0��, the transmission of the following information will contain data bytes only.  
  *         D/C#�� The D/C# bit determines the next data byte is acted as a command or a data. If the D/C# bit is 
  *         set to logic ��0��, it defines the following data byte as a command. If the D/C# bit is set to 
  *         logic ��1��, it defines the following data byte as a data which will be stored at the GDDRAM. 
  *         The GDDRAM column address pointer will be increased by one automatically after each 
  *         data write. 
  *         �����������Щ��������Щ��������Щ��������Щ��������Щ��������Щ��������Щ��������Щ�������
  *         �� b7 �� b6 �� b5 �� b4 �� b3 �� b2 �� b1 �� b0 ��   ��
  *         �����������੤�������੤�������੤�������੤�������੤�������੤�������੤��������ACK��
  *         �� Co ��D/C#�� 0  �� 0  �� 0  �� 0  �� 0  �� 0  ��   ��
  *         �����������ة��������ة��������ة��������ة��������ة��������ة��������ة��������ة�������
  *  
  * *********************************************************
  * 
  */
void OLED_WriteCmd(uint8_t cmd)
{
    OLED_I2C_Start();
    OLED_SendByte(0x78);    //��ַ�ֽڣ�����slave(�ӻ�)��ַ
    OLED_SendByte(0X00);    //�����ֽڣ�׼��дָ�Co = 0, D/C# = 0
    OLED_SendByte(cmd);     //дָ��
    OLED_I2C_Stop();
}

void OLED_WriteData(uint8_t data)
{
    OLED_I2C_Start();
    OLED_SendByte(0x78);    //��ַ�ֽڣ�����slave(�ӻ�)��ַ
    OLED_SendByte(0X40);    //�����ֽڣ�׼��д���ݣ�Co = 0, D/C# = 1
    OLED_SendByte(data);    //д����
    OLED_I2C_Stop();
}


/**
  * @brief  OLED���ù��λ��
  * @param  Y �����Ͻ�Ϊԭ�㣬���·�������꣬��Χ��0~7
  * @param  X �����Ͻ�Ϊԭ�㣬���ҷ�������꣬��Χ��0~127
  * @retval ��
  * 
  * **************************************************
  *     128*64�ĵ��󣬴��з�������64�еĵ���ֳ�Ϊ��8��PAGE����PAGE0~PAGE7����Ҳ��Ϊʲôyȡ0~7
  *     ���з���������128�еĵ��󣬼�SEG0~SGE127������xȡֵ0~127
  * 
  */
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
	OLED_WriteCmd(0xB0 | Y);					//����PAGE��ʼ��ַ
	OLED_WriteCmd(0x10 | ((X & 0xF0) >> 4));	//ȡx�ĸ�4λ������4λ����̶���0x10��λ�������е�ַ��4λ
	OLED_WriteCmd(0x00 | (X & 0x0F));			//ȡx�ĵ�4λ�����е�ַ��4λ����̶���0x00��λ�������е�ַ��4λ
}

/* OLED����  */
void OLED_Clear(void)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        OLED_SetCursor(i, 0);   //������õ���i�е�1��
        for (uint8_t j  = 0; j < 128; j++)
        {
            OLED_WriteData(0X00);
        } 
    }
}
