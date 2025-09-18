#include "timer.h"

uint16_t CCR1_Val = 50;

void Timer_Init(void)
{
	/* 开启时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB2Periph_GPIOA, ENABLE);
	
    /* 配置GPIO */
    GPIO_InitTypeDef GPIO_InitStructure;

	/*	配置ETR对应的GPIO */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/**
	 * 使用外部时钟模式2：不分频、极性不翻转、触发滤波器值最大0X0F
	 */
	// TIM_ETRClockMode2Config(TIM2, TIM_ExtTRGPSC_OFF, TIM_ExtTRGPolarity_NonInverted, 0X0F);
	TIM_InternalClockConfig(TIM2);

	/**
	 * 时基单元初始化：
	 * CK_INT不分频、向上计数模式、ARR自动重载值10、预分频系数1、重复计数器=0(高级定时器才有)
	 * TIM_ClockDivision对CK_INT进行分频，分频后的时钟用于ETR，TIx的数字滤波器
	 * 	
	 */
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 720 - 1;		//72 000 000 / 72 / 100=10000Hz = 0.1 ms
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

    /* 为TIM_OCInitStructure每个成员设置默认值，有些结构体成员对TIM2无效 */
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure);

	/**
	 * 定时器设置为PWM模式1、TIM_Pulse即CCRx寄存器的值、极性不翻转
	 * 
	 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM2, &TIM_OCInitStructure);

	/* TIM2中断配置 */
	// TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	/* NVIC配置 */
	// NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
	// NVIC_InitTypeDef NVIC_InitStructure;
	// NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	// NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	// NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	// NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	// NVIC_Init(&NVIC_InitStructure);

	/* 打开定时器 */
	TIM_Cmd(TIM2, ENABLE);
}

/* 定时器2中端处理函数 */
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		// num++;
		// printf("%d", num);
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);	//清除TIM2更新事件的中断标志位
	}
}
