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
		OLED_ShowNum(1, 5, num, 5);			//不断刷新显示Num变量
		OLED_ShowNum(2, 1, TIM_GetCounter(TIM2), 5);
	}
}

void Timer_Init(void)
{
	/* 开启时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB2Periph_GPIOA, ENABLE);
	
    /* 配置GPIO */
    GPIO_InitTypeDef GPIO_InitStructure;

	/*	配置ETR对应的GPIO */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/**
	 * 使用外部时钟模式2：不分频、极性不翻转、触发滤波器值最大0X0F
	 */
	TIM_ETRClockMode2Config(TIM2, TIM_ExtTRGPSC_OFF, TIM_ExtTRGPolarity_NonInverted, 0X0F);


	/**
	 * 时基单元初始化：
	 * CK_INT不分频、向上计数模式、ARR自动重载值10、预分频系数1、重复计数器=0(高级定时器才有)
	 * TIM_ClockDivision对CK_INT进行分频，分频后的时钟用于ETR，TIx的数字滤波器
	 * 	
	 */
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 10 - 1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1;		
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

	/* TIM2中断配置 */
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	/* NVIC配置 */
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* 打开定时器 */
	TIM_Cmd(TIM2, ENABLE);
}

/* 定时器2中端处理函数 */
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		num++;
		printf("%d", num);
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);	//清除TIM2更新事件的中断标志位
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
