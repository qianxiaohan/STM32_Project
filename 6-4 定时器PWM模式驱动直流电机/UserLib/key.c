#include "key.h"
#include "delay.h"

void Key_Init(void)
{
    /* 开启GPIOA时钟、定时器TIM2时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA, ENABLE);

    /* 配置GPIO */    
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
