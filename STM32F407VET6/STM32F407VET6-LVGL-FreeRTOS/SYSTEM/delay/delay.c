//使用定时器实现延时函数
#include "delay.h"

void delay_us(u16 nus)
{	
	//定时器计数周期1us
	TIM_SetCounter(TIM3, 0);	//重置计数器
	TIM_Cmd(TIM3, ENABLE);	//打开计数器
	while(TIM_GetCounter(TIM3) < nus);	//等待计数完成
	TIM_Cmd(TIM3, DISABLE);	//关闭计数器
}

void delay_ms(u16 nms)
{
	while (nms--)
	{
		delay_us(1000);
	}
	
}

