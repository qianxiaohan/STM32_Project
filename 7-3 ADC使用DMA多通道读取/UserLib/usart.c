#include "usart.h"
#include <stdio.h>

uint8_t receiveBuffer[256];
uint8_t receiveCounter;

void COM_Init(void)
{
	/* 开启时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    /* 配置GPIO */
    GPIO_InitTypeDef GPIO_InitStructure;

	/*	配置USARTx Tx的GPIO口 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/*	配置USARTx Rx的GPIO口 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//复用浮空输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/**
	 *  串口参数配置：
	 * 	波特率 115200、无硬件控制流、无校验位、8位数据位、1位停止位、发送/接收模式
	 *  
	 */
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

	/* 配置NVIC */
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* 使能接收中断 */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART1, ENABLE);
}

void COM_SendData(uint8_t data)
{
    USART_SendData(USART1, data);

	/**
	 *  TXE
	 *   0: Data is not transferred to the shift register
     *   1: Data is transferred to the shift register
	 * 	
	 * 	TXE = 1：发送完成， TXE = 0：还未发送完成
	 * 
	 */
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	
}

/* USART1中断处理函数 */
void USART1_IRQHandler(void)
{
	/**
	 * 判断中断源是否是接收中断
	 * 返回SET则说明是接收中断
	 */
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		/* 读取数据寄存器中的数据， */
		uint8_t tempdata = USART_ReceiveData(USART1);
		receiveBuffer[receiveCounter++] = tempdata;
		COM_SendData(tempdata);
	}
}


int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t)ch);
	/* 硬件会自动置1 */
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)	
	{
		// USART_ClearFlag(USART1, USART_FLAG_TXE);
	}

	return ch;
}
