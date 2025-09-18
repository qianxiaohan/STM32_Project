#include "i2c.h"
#include "stm32f10x.h"

#define _OK 0
#define _ERROR 1
#define LONG_TIMEOUT 2000

uint16_t Timeout;

/* 包括GPIO、I2C外设的初始化 */
void MyI2C_init(void)
{
	/* 开启外设时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	
    /* 使用I2C外设，GPIO必须配置为复用开漏输出 */
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* i2c外设初始化 */
    I2C_InitTypeDef I2C_InitStructure;
    I2C_InitStructure.I2C_Mode = I2C_Mode_SMBusDevice;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_OwnAddress1 = SLAVE_ADDRESS;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_ClockSpeed = 100000;
    I2C_Cmd(I2C1, ENABLE);
    I2C_Init(I2C1, &I2C_InitStructure);
}

/**
 * @brief I2C外设发送数据
 * @param data 数据数组
 * @param len 数组长度
 * 
 * ****************************
 * 
 * I2C_CheckEvent用来检测每次发送完一个字节后的状态，参照《STM32F10x参考手册》、以及《STM32F103xx固件函数库手册》
 * OK = 0, ERROR = 1
 * 
 */
uint8_t MyI2C_SendData(uint8_t *data, uint8_t len)
{
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if((Timeout--) == 0) return _ERROR;
    }
    I2C_Send7bitAddress(I2C1, SLAVE_ADDRESS, I2C_Direction_Transmitter);    //发送7位地址
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if((Timeout--) == 0) return _ERROR;
    }
    for(int i = 0; i < len; i++)
    {
        I2C_SendData(I2C1, data[i]);
        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        {
            if((Timeout--) == 0) return _ERROR;
        }
    }
    I2C_GenerateSTOP(I2C1, ENABLE);
    return _OK;
}

uint8_t MyI2C_ReadData(uint8_t *data)
{
    /* 发送I2C开始条件*/
    I2C_GenerateSTART(I2C1, ENABLE);

    /* 测试EV5 */    
    Timeout = LONG_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) 
    {
        if((Timeout--) == 0) return _ERROR;
    }

    /* 发送从机地址，方向为接收 */
    I2C_Send7bitAddress(I2C1, SLAVE_ADDRESS, I2C_Direction_Receiver);   

    /* 测试EV6 */
    Timeout = LONG_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))    
    {
        if((Timeout--) == 0) return _ERROR;
    }

    /* 失能ACK */
    I2C_AcknowledgeConfig(I2C1, DISABLE); 

    /* 发送I2C停止条件 */  
    I2C_GenerateSTOP(I2C1, ENABLE);     

    /* 测试EV7 */
    Timeout = LONG_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))   
    {
        if((Timeout--) == 0) return _ERROR;
    }

    /* 接收一个字节的数据 */
    *data = I2C_ReceiveData(I2C1); 

    /* 将应答恢复为使能，为了不影响后续可能产生的读取多字节操作 */
    I2C_AcknowledgeConfig(I2C1, ENABLE);

    return _OK;
}

