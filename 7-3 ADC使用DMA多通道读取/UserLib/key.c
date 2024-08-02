#include "key.h"
#include "delay.h"
#include <stdio.h>

void Key_Init(void)
{
    /* 开启GPIOA时钟、定时器TIM2时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

    /* 配置GPIO */    
    GPIO_InitTypeDef GPIOInitStructure;
    GPIOInitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIOInitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIOInitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(GPIOB, &GPIOInitStructure);

    /* GPIO配置外部中断线 */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource11);

    /* 配置外部中断结构体 */
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_Line11;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Event;     //外部事件模式
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿触发
    EXTI_Init(&EXTI_InitStructure);

    // /* 配置NVIC */
    // NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);

    // NVIC_InitTypeDef NVIC_InitStructure;
    // NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    // NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    // NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    // NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    // NVIC_Init(&NVIC_InitStructure);
}

uint8_t getKeyNum(void)
{
    if(GPIO_ReadInputDataBit(GPIOB, KEY1) == RESET)
    {
        delay_ms(20);
        while(GPIO_ReadInputDataBit(GPIOB, KEY1) == RESET);		//等待按键松手
        delay_ms(20);
        return 1;
    }
    if(GPIO_ReadInputDataBit(GPIOA, KEY2) == RESET)
    {
        delay_ms(20);
        while(GPIO_ReadInputDataBit(GPIOA, KEY2) == RESET);		//等待按键松手
        delay_ms(20);
        return 2;
    }
    if(GPIO_ReadInputDataBit(GPIOB, KEY3) == RESET)
    {
        delay_ms(20);
        while(GPIO_ReadInputDataBit(GPIOB, KEY3) == RESET);		//等待按键松手
        delay_ms(20);
        return 3;
    }
    return 0;
}

/* 外部中断线EXIT11中断处理函数 */
void EXTI15_10_IRQHandler(void)
{
    printf("hello");
}