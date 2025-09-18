#include "usart.h"
#include <stdio.h>

uint8_t receiveBuffer[256];
uint8_t receiveCounter;

void COM_Init(void)
{
	/* ����ʱ�� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    /* ����GPIO */
    GPIO_InitTypeDef GPIO_InitStructure;

	/*	����USARTx Tx��GPIO�� */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/*	����USARTx Rx��GPIO�� */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//���ø�������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/**
	 *  ���ڲ������ã�
	 * 	������ 115200����Ӳ������������У��λ��8λ����λ��1λֹͣλ������/����ģʽ
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

	/* ����NVIC */
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* ʹ�ܽ����ж� */
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
	 * 	TXE = 1��������ɣ� TXE = 0����δ�������
	 * 
	 */
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	
}

/* USART1�жϴ����� */
void USART1_IRQHandler(void)
{
	/**
	 * �ж��ж�Դ�Ƿ��ǽ����ж�
	 * ����SET��˵���ǽ����ж�
	 */
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		/* ��ȡ���ݼĴ����е����ݣ� */
		uint8_t tempdata = USART_ReceiveData(USART1);
		receiveBuffer[receiveCounter++] = tempdata;
		COM_SendData(tempdata);
	}
}

/* printf�����ض��� */
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t)ch);
	/* Ӳ�����Զ���1 */
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)	
	{
		// USART_ClearFlag(USART1, USART_FLAG_TXE);
	}

	return ch;
}
