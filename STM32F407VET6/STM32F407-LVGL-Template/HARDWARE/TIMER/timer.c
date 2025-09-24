/**
 ****************************************************************************************************
 * @file        time.c
 * @brief       通用定时器中断初始化代码(包括时钟配置/中断管理/GPIO设置等)
 ****************************************************************************************************
 * @attention
 * @note:     在使用此头文件时，需确保相关的依赖项已正确配置，
 * @contact:  微信公众号 - [GenBotter]，分享技术内容
 *            QQ群号 - [366182133]，在线技术支持
 *            淘宝店铺 - [https://genbotter.taobao.com]，提供配套硬件产品
 ****************************************************************************************************
 */
#include "timer.h"
#include "stm32f4xx.h"
#include "lvgl.h"
#include "usart.h"

unsigned char flag_5ms;

//////////////////////////////////////////////////////////////////////////////////	 
//通用定时器3中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!
void TIM3_Int_Init(uint16_t arr,uint16_t psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///使能TIM3时钟
	
	TIM_InternalClockConfig(TIM3);

    TIM_TimeBaseInitStructure.TIM_Period = arr; 	//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);//初始化TIM3
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //允许定时器3更新中断
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_Cmd(TIM3,ENABLE); //使能定时器3
}

//定时器3中断服务函数
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
	{
		static int count;
		count ++;
		if(count > 5)
		{
				count = 0;
				flag_5ms = 1;
		}
		lv_tick_inc(1);	//进入中断的ms数要和本函数的输入参数相同
		// USART_SendData(USART1, 'A');
		// USART_SendData(USART1, 0xff);
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位
	}
	
}
