#include "key.h"
#include "delay.h"

void Key_Init(void)
{
    /* ����GPIOAʱ�ӡ���ʱ��TIM2ʱ�� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA, ENABLE);

    /* ����GPIO */    
    GPIO_InitTypeDef GPIOInitStructure;
    GPIOInitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIOInitStructure.GPIO_Pin = KEY1 | KEY3;
    GPIOInitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIOInitStructure);

    GPIOInitStructure.GPIO_Pin = KEY2;
    GPIO_Init(GPIOA, &GPIOInitStructure);
}

uint8_t getKeyNum(void)
{
    if(GPIO_ReadInputDataBit(GPIOB, KEY1) == RESET)
    {
        delay_ms(20);
        while(GPIO_ReadInputDataBit(GPIOB, KEY1) == RESET);		//�ȴ���������
        delay_ms(20);
        return 1;
    }
    if(GPIO_ReadInputDataBit(GPIOA, KEY2) == RESET)
    {
        delay_ms(20);
        while(GPIO_ReadInputDataBit(GPIOA, KEY2) == RESET);		//�ȴ���������
        delay_ms(20);
        return 2;
    }
    if(GPIO_ReadInputDataBit(GPIOB, KEY3) == RESET)
    {
        delay_ms(20);
        while(GPIO_ReadInputDataBit(GPIOB, KEY3) == RESET);		//�ȴ���������
        delay_ms(20);
        return 3;
    }
    return 0;
}
