#include "stm32f10x.h"                  // Device header

int main(void)
{
	/* 配置时钟 外设GPIO位于APB2上 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	/* 外设结构体初始化 */
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOC, GPIO_Pin_13);

	while (1)
	{
		
	}
}
