#include "timer.h"

uint16_t CCR1_Val = 50;

void Timer_Init(void)
{
	/* ����ʱ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB2Periph_GPIOA, ENABLE);
	
    /* ����GPIO */
    GPIO_InitTypeDef GPIO_InitStructure;

	/*	����ETR��Ӧ��GPIO */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/**
	 * ʹ���ⲿʱ��ģʽ2������Ƶ�����Բ���ת�������˲���ֵ���0X0F
	 */
	// TIM_ETRClockMode2Config(TIM2, TIM_ExtTRGPSC_OFF, TIM_ExtTRGPolarity_NonInverted, 0X0F);
	TIM_InternalClockConfig(TIM2);

	/**
	 * ʱ����Ԫ��ʼ����
	 * CK_INT����Ƶ�����ϼ���ģʽ��ARR�Զ�����ֵ10��Ԥ��Ƶϵ��1���ظ�������=0(�߼���ʱ������)
	 * TIM_ClockDivision��CK_INT���з�Ƶ����Ƶ���ʱ������ETR��TIx�������˲���
	 * 	
	 */
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 720 - 1;		//72 000 000 / 72 / 100=10000Hz = 0.1 ms
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

    /* ΪTIM_OCInitStructureÿ����Ա����Ĭ��ֵ����Щ�ṹ���Ա��TIM2��Ч */
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure);

	/**
	 * ��ʱ������ΪPWMģʽ1��TIM_Pulse��CCRx�Ĵ�����ֵ�����Բ���ת
	 * 
	 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM2, &TIM_OCInitStructure);

	/* TIM2�ж����� */
	// TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	/* NVIC���� */
	// NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
	// NVIC_InitTypeDef NVIC_InitStructure;
	// NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	// NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	// NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	// NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	// NVIC_Init(&NVIC_InitStructure);

	/* �򿪶�ʱ�� */
	TIM_Cmd(TIM2, ENABLE);
}

/* ��ʱ��2�ж˴����� */
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		// num++;
		// printf("%d", num);
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);	//���TIM2�����¼����жϱ�־λ
	}
}
