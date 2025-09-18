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
		
	}
}

void Timer_Init(void)
{
	/* ����ʱ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	/* ���ô���ģʽ��ֱ�����ڲ�ʱ��TIMx_CLKΪԤ��Ƶ����ʱ */
	TIM_InternalClockConfig(TIM2);

	/**
	 * ʱ����Ԫ��ʼ����
	 * CK_INT����Ƶ�����ϼ���ģʽ��ARR�Զ�����ֵ9999��Ԥ��Ƶϵ��7199���ظ�������=0(�߼���ʱ������)	
	 */
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 10000 - 1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7200 - 1;		
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

	/**
	 *  ��Ҫ��ʱ1s�������������72MHz / ��Ƶ
	 *  ��ʽ��CK_CNT_OV = CK_PSC / (PSC + 1) / (ARR + 1)
	 * 	���� 72MHz / 7200 / 10000 = 1Hz = 1s
	 * 
	 */


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
