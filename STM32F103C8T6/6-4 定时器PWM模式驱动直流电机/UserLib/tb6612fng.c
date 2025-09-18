#include "tb6612fng.h"

void Motor_Driver_Init(void)
{
    /* ����GPIOAʱ�ӡ���ʱ��TIM2ʱ�� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    /* ����GPIO */    
    GPIO_InitTypeDef GPIOInitStructure;
    GPIOInitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOInitStructure.GPIO_Pin = AIN1_Pin | AIN2_Pin;
    GPIOInitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIOInitStructure);

    /* ����TIM2��ͨ����Ӧ��GPIO */
    GPIOInitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIOInitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOA, &GPIOInitStructure);

    /* �ڲ�ʱ����Ϊ��ʱ������ */
    TIM_InternalClockConfig(TIM2);

    /* ����TIM2 */
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseInitStructure.TIM_Period = 1000 - 1;
    TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

     /* Ϊ����Ƚ�(OC)�ṹ��TIM_OCInitStructureÿ����Ա����Ĭ��ֵ����Щ�ṹ���Ա��TIM2��Ч */
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure);

	/**
	 * ��ʱ������ΪPWMģʽ1��TIM_Pulse��CCRx�Ĵ�����ֵ�����Բ���ת
	 * 
	 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_Pulse = 400;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM2, &TIM_OCInitStructure);

    /* �򿪶�ʱ�� */
	TIM_Cmd(TIM2, ENABLE);

    /* ���ֹͣ */
    Motor_SetMode(STOP);
}

void Motor_SetMode(MODE mode)
{
    switch (mode)
    {
    case SHORT_BRAKE:
        AIN1_SetBit(1);
        AIN2_SetBit(1);
        break;
    case CCW:
        AIN1_SetBit(0);
        AIN2_SetBit(1);
        break;
    case CW:
        AIN1_SetBit(1);
        AIN2_SetBit(0);
        break;
    case STOP:
        AIN1_SetBit(0);
        AIN2_SetBit(0);
        break;
    default:
        break;
    }
}

void Motor_SetSpeed(int speed)
{
    TIM_SetCompare1(TIM2, speed);
}
