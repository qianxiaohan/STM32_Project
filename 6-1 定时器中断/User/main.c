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
	/* 开启时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	/* 禁用从属模式，直接用内部时钟TIMx_CLK为预分频器计时 */
	TIM_InternalClockConfig(TIM2);

	/**
	 * 时基单元初始化：
	 * CK_INT不分频、向上计数模式、ARR自动重载值9999、预分频系数7199、重复计数器=0(高级定时器才有)	
	 */
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 10000 - 1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7200 - 1;		
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

	/**
	 *  需要定时1s，计数器溢出，72MHz / 分频
	 *  公式：CK_CNT_OV = CK_PSC / (PSC + 1) / (ARR + 1)
	 * 	带入 72MHz / 7200 / 10000 = 1Hz = 1s
	 * 
	 */


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
