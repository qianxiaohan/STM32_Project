//ʹ�ö�ʱ��ʵ����ʱ����
#include "delay.h"

void delay_us(u16 nus)
{	
	//��ʱ����������1us
	TIM_SetCounter(TIM3, 0);	//���ü�����
	TIM_Cmd(TIM3, ENABLE);	//�򿪼�����
	while(TIM_GetCounter(TIM3) < nus);	//�ȴ��������
	TIM_Cmd(TIM3, DISABLE);	//�رռ�����
}

void delay_ms(u16 nms)
{
	while (nms--)
	{
		delay_us(1000);
	}
	
}

