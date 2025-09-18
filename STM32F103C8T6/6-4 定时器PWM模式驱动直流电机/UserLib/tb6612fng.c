#include "tb6612fng.h"

void Motor_Driver_Init(void)
{
    /* 开启GPIOA时钟、定时器TIM2时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    /* 配置GPIO */    
    GPIO_InitTypeDef GPIOInitStructure;
    GPIOInitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOInitStructure.GPIO_Pin = AIN1_Pin | AIN2_Pin;
    GPIOInitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIOInitStructure);

    /* 配置TIM2对通道对应的GPIO */
    GPIOInitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIOInitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOA, &GPIOInitStructure);

    /* 内部时钟作为定时器输入 */
    TIM_InternalClockConfig(TIM2);

    /* 配置TIM2 */
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseInitStructure.TIM_Period = 1000 - 1;
    TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

     /* 为输出比较(OC)结构体TIM_OCInitStructure每个成员设置默认值，有些结构体成员对TIM2无效 */
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure);

	/**
	 * 定时器设置为PWM模式1、TIM_Pulse即CCRx寄存器的值、极性不翻转
	 * 
	 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_Pulse = 400;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM2, &TIM_OCInitStructure);

    /* 打开定时器 */
	TIM_Cmd(TIM2, ENABLE);

    /* 电机停止 */
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
