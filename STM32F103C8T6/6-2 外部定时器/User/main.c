#include "stm32f10x.h"                  // Device header
#include "oled.h"
#include <stdio.h>
#include "usart.h"

uint8_t num;

void Timer_Init(void);

int main(void)
{
	OLED_Init();
	COM_Init();
	Timer_Init();

	// printf("1");
	while (1)
	{
		OLED_ShowNum(1, 5, num, 5);			//����ˢ����ʾNum����
		OLED_ShowNum(2, 1, TIM_GetCounter(TIM2), 5);
	}
}

void Timer_Init(void)
{
	/* ����ʱ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB2Periph_GPIOA, ENABLE);
	
    /* ����GPIO */
    GPIO_InitTypeDef GPIO_InitStructure;

	/*	����ETR��Ӧ��GPIO */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/**
	 * ʹ���ⲿʱ��ģʽ2������Ƶ�����Բ���ת�������˲���ֵ���0X0F
	 */
	TIM_ETRClockMode2Config(TIM2, TIM_ExtTRGPSC_OFF, TIM_ExtTRGPolarity_NonInverted, 0X0F);


	/**
	 * ʱ����Ԫ��ʼ����
	 * CK_INT����Ƶ�����ϼ���ģʽ��ARR�Զ�����ֵ10��Ԥ��Ƶϵ��1���ظ�������=0(�߼���ʱ������)
	 * TIM_ClockDivision��CK_INT���з�Ƶ����Ƶ���ʱ������ETR��TIx�������˲���
	 * 	
	 */
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 10 - 1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1;		
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

	/* TIM2�ж����� */
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	/* NVIC���� */
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* �򿪶�ʱ�� */
	TIM_Cmd(TIM2, ENABLE);
}

/* ��ʱ��2�ж˴����� */
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		num++;
		printf("%d", num);
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);	//���TIM2�����¼����жϱ�־λ
	}
}

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
