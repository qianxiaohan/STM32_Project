#include "stm32f10x.h"                  // Device header
#include "oled.h"
#include <stdio.h>
#include "usart.h"
#include "timer.h"
#include "led.h"
#include "delay.h"

uint8_t num;

void Timer_Init(void);

int main(void)
{
	LED_Init();
	Timer_Init();
	while (1)
	{
		for(int i = 0; i < 100; i++)
		{
			TIM_SetCompare1(TIM2, i);
			delay_ms(10);

		}
		for(int i = 99; i >= 0; i--)
		{
			TIM_SetCompare1(TIM2, i);
			delay_ms(10);
		}	
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
